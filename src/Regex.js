/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 * Approximate JavaScript regular expression to Z3 regex parser
 */
function CullOuterRegex(regex) {
    let firstSlash = regex.indexOf('/');
    let lastSlash = regex.lastIndexOf('/');
    return regex.substr(firstSlash + 1, lastSlash - 1);
}

let REGEX_CTR = 0;

function RegexRecursive(ctx, regex, idx) {

    REGEX_CTR++;

    let captures = [];
    let assertions = [];
    let fill_ctr = 0;

    function nextFiller() {
        return ctx.mkStringVar('' + regex + REGEX_CTR + ' Fill ' + fill_ctr++);
    }

    function more() {
        return idx < regex.length && current() != '|' && current() != ')';
    }

    function mk(v) {
        return ctx.mkSeqToRe(ctx.mkString(v));
    }

    function current() {
        return regex[idx];
    }

    function next() {
        return regex[idx++];
    }

    function peek() {
        return regex[idx + 1];
    }

    function Any() {
        let beforeNewline = ctx.mkReRange(ctx.mkString('\\x00'), ctx.mkString('\\x09'));
        let afterNewline = ctx.mkReRange(ctx.mkString('\\x0B'), ctx.mkString('\\xFF'));
        return ctx.mkReUnion(beforeNewline, afterNewline);
    }

    function ParseRangeInner() {

        let union = undefined;

        while (more() && current() != ']') {
            let c1 = ctx.mkString(next());
            let range = undefined;

            if (current() == '-') {
                next();
                let c2 = ctx.mkString(next());
                range = ctx.mkReRange(c1, c2);
            } else {
                range = ctx.mkSeqToRe(c1);
            }

            if (!union) {
                union = range;
            } else {
                union = ctx.mkReUnion(union, range);
            }
        }

        return union;
    }

    function ParseRange() {
        next();

        let negate = false;

        if (current() == '^') {
            next();
            negate = true;
        }

        let r = ParseRangeInner();

        if (negate) {
            let comp = ctx.mkReComplement(r);
            r = ctx.mkReIntersect(Any(), comp);
        }

        if (next() == ']') {
            return r;
        } else {
            return null;
        }
    }

    let Specials = {
        '.': Any
    }

    function Alpha() {
        let p1 = ctx.mkReRange(ctx.mkString('a'), ctx.mkString('z'));
        let p2 = ctx.mkReRange(ctx.mkString('A'), ctx.mkString('Z'));
        return ctx.mkReUnion(p1, p2);
    }

    function Digit() {
        return ctx.mkReRange(ctx.mkString('0'), ctx.mkString('9'));
    }

    function Whitespace() {
        let p1 = mk(' ');
        let p2 = mk('\t');
        let p3 = mk('\r');
        let p4 = mk('\n');
        let p5 = mk('\f');
        return ctx.mkReUnion(p1, ctx.mkReUnion(p2, ctx.mkReUnion(p3, ctx.mkReUnion(p4, p5))));
    }

    function AlphaNumeric() {
        return ctx.mkReUnion(Alpha(), Digit());
    }

    function Word() {
        return ctx.mkReUnion(AlphaNumeric(), mk('_'));
    }

    function ParseAtom1() {
        return mk(next());
    }

    function ParseMaybeSpecial() {
        if (Specials[current()]) {
            return Specials[next()]();
        } else {
            return ParseAtom1();
        }
    }

    function ParseMaybeEscaped() {
        if (current() == '\\') {
            next();

            let c = next();

            if (c == 'd') {
                return Digit();
            } else if (c == 'D') {
                return ctx.mkReComplement(Digit());
            } else if (c == 'w') {
                return Word();
            } else if (c == 'W') {
                return ctx.mkReComplement(Word());
            } else if (c == 's') {
                return Whitespace();
            } else if (c == 'S') {
                return ctx.mkReComplement(Whitespace());
            } else if (c == 'n') {
                return mk('\n');
            } else if (c == 'r') {
                return mk('\r');
            } else if (c == 't') {
                return mk('\t');
            } else if (c == '0') {
                return mk('\\x00');
            }

            return mk(c);
        } else {
            return ParseMaybeSpecial();
        }
    }

    function ParseMaybeRange() {
        if (current() == '[') {
            let range = ParseRange();
            if (!range) {
                return null;
            }
            return range;
        } else {
            return ParseMaybeEscaped();
        }
    }

    function rewriteCaptureOptional(idx) {
        //Rewrite capture[n] to be capture[n] or ''
        let orFiller = nextFiller();
        assertions.push(ctx.mkOr(ctx.mkEq(orFiller, captures[idx]), ctx.mkEq(orFiller, ctx.mkString(''))));
        captures[idx] = orFiller;
    }

    function ParseMaybeCaptureGroupStart(captureIndex) {
        if (current() == '(') {
            next();

            let capture = true;

            //Ignore ?: capture groups can't be modelled
            if (current() == '?') {
                next();

                if (current() != ':') {
                    return null;
                }

                next();
                capture = false;
            }

            let atoms = ParseCaptureGroup();

            if (next() != ')') {
                return null;
            }

            function addToCapture(idx, thing) {
                captures[idx] = ctx.mkSeqConcat([captures[idx], thing]);
            }

            function handlePlusRewriting(atoms, plusGroup) {
                let ncap = captures[plusGroup];

                atoms = ctx.mkRePlus(atoms);

                let outerFiller = nextFiller();
                assertions.push(ctx.mkSeqInRe(outerFiller, atoms));

                let innerFiller = nextFiller();
                assertions.push(ctx.mkEq(outerFiller, ctx.mkSeqConcat([innerFiller, ncap])));

                addToCapture(captureIndex, outerFiller);
                return atoms;
            }

            function handleStarRewriting(atoms, starGroup) {
                let ncap = captures[starGroup];

                atoms = ctx.mkReStar(atoms);

                let outerFiller = nextFiller();
                assertions.push(ctx.mkSeqInRe(outerFiller, atoms));
                assertions.push(ctx.mkImplies(ctx.mkEq(ncap, ctx.mkString('')), ctx.mkEq(outerFiller, ctx.mkString(''))));

                let innerFiller = nextFiller();
                assertions.push(ctx.mkEq(outerFiller, ctx.mkSeqConcat([innerFiller, ncap])));

                addToCapture(captureIndex, outerFiller);
                return atoms;
            }

            function handleOptionRewriting(atoms, optionGroup) {
                atoms = ctx.mkReOption(atoms);
                addToCapture(captureIndex, optionGroup);
                return atoms;
            }

            if (capture) {
                let newestCapture = captures.length - 1;
                switch (current()) {
                    case '*':
                        {
                            rewriteCaptureOptional(newestCapture);
                            atoms = handleStarRewriting(atoms, newestCapture);

                            next();
                            break;
                        }
                    case '+':
                        {
                            atoms = handlePlusRewriting(atoms, newestCapture);
                            next();
                            break;
                        }
                    case '?':
                        {
                            rewriteCaptureOptional(newestCapture, newestCapture);
                            atoms = handleOptionRewriting(atoms);
                            next();
                            break;
                        }
                    default:
                        {
                            addToCapture(captureIndex, captures[newestCapture]);
                        }
                }
            }

            return atoms;
        } else {
            return ParseMaybeRange();
        }
    }

    function ParseMaybePSQ(captureIndex) {

        let atom = ParseMaybeCaptureGroupStart(captureIndex);

        if (!atom) {
            return null;
        }

        if (current() == '*') {
            next();
            atom = ctx.mkReStar(atom);
        } else if (current() == '+') {
            next();
            atom = ctx.mkRePlus(atom);
        } else if (current() == '?') {
            next();
            atom = ctx.mkReOption(atom);
        }

        return atom;
    }

    function ParseNumber() {

        function digit() {
            return current() >= '0' && current() <= '9';
        }

        let numStr = '';

        if (!digit()) {
            return null;
        }

        while (digit()) {
            numStr += next();
        }

        return parseInt(numStr);
    }

    function ParseLoopCount() {
        let n1 = ParseNumber();

        if (n1 === null) {
            return [null, null];
        }

        if (current() == ',') {
            next();
            let n2 = ParseNumber();
            return [n1, n2];
        } else {
            return [n1, n1];
        }
    }

    function ParseMaybeLoop(captureIndex) {
        let atom = ParseMaybePSQ(captureIndex);

        if (current() == '{') {
            next();

            let [lo, hi] = ParseLoopCount();

            if (lo === null || hi === null) {
                return null;
            }

            if (!next() == '}') {
                return null;
            }

            atom = ctx.mkReLoop(atom, lo, hi);
        }

        return atom;
    }

    function ParseMaybeAtoms(captureIndex) {

        let rollup = null;

        while (more()) {

            while (current() == '^' || current() == '$') {
                //TODO: Find out how to handle multiline
                next();
            }

            //TODO: This is horrible, anchors should be better
            if (more()) {
                let parsed = ParseMaybeLoop(captureIndex);

                if (!parsed) {
                    return null;
                }

                rollup = rollup ? ctx.mkReConcat(rollup, parsed) : parsed;
            }
        }

        return rollup.simplify();
    }

    function buildAlternationCaptureConstraints(captureIndex, startCaptures, endLeftCaptures, endRightCaptures, cLeft, cRight) {
        let leftCaptures = [];
        let leftOriginals = [];
        let rightCaptures = [];
        let rightOriginals = [];

        for (let i = startCaptures; i < endLeftCaptures; i++) {
            leftOriginals.push(captures[i]);
            rewriteCaptureOptional(i);
            leftCaptures.push(captures[i]);
        }

        for (let i = endLeftCaptures; i < endRightCaptures; i++) {
            rightOriginals.push(captures[i]);
            rewriteCaptureOptional(i);
            rightCaptures.push(captures[i]);
        }
        
        let cFinal = nextFiller();

        function buildSide(side, left, original, right) {
            let forceRightNothing = right.map(x => ctx.mkEq(x, ctx.mkString('')));
            let forceLeftOriginal = left.map((x, idx) => ctx.mkEq(left[idx], original[idx]));
            assertions.push(ctx.mkImplies(ctx.mkEq(cFinal, side), ctx.mkAndList(forceRightNothing.concat(forceLeftOriginal))));
        }

        buildSide(cLeft, leftCaptures, leftOriginals, rightCaptures);
        buildSide(cRight, rightCaptures, rightOriginals, leftCaptures);

        assertions.push(ctx.mkOr(ctx.mkEq(cFinal, cLeft), ctx.mkEq(cFinal, cRight)));
        captures[captureIndex] = cFinal;
    }

    function ParseMaybeOption(captureIndex) {

        //Track the length of captures through parsing of either side
        //If it changes then the blocks parsed have a capture in them
        //and will need extra constraints
        let startCaptures = captures.length;

        //The captures on an option are a bit tricky
        //The capture is either going to be the current C0 + [Some stuff] | [Some Stuff]
        //So we parse one side, reset the capture to cStart, then parse the other and express
        //the final constraint as an or of the two
        let cStart = captures[captureIndex];

        let ast = ParseMaybeAtoms(captureIndex);

        let cLeft = captures[captureIndex];
        captures[captureIndex] = cStart;

        let endLeftCaptures = captures.length;

        if (!ast) {
            return null;
        }

        if (current() == '|') {
            next();

            let ast2 = ParseMaybeOption(captureIndex);

            let cRight = captures[captureIndex];

            let endRightCaptures = captures.length;

            //If any capture groups have been defined in the alternation we need to
            //build some new string constraints on the result
            if (endRightCaptures != startCaptures) {
                buildAlternationCaptureConstraints(captureIndex, startCaptures, endLeftCaptures, endRightCaptures, cLeft, cRight);
            }

            if (!ast2) {
                return null;
            }

            return ctx.mkReUnion(ast, ast2);
        } else {
            return ast;
        }
    }

    function ParseCaptureGroup() {
        let captureIndex = captures.length;
        captures.push(nextFiller());

        let r = ParseMaybeOption(captureIndex);

        if (!r) {
            return null;
        }

        assertions.push(ctx.mkSeqInRe(captures[captureIndex], r));

        return r;
    }

    let ast = ParseCaptureGroup();

    let implier = captures[0];

    if (regex[0] !== '^') {
        ast = ctx.mkReConcat(ctx.mkReStar(Any()), ast);
        implier = ctx.mkSeqConcat([nextFiller(), implier]);
    }

    if (regex[regex.length - 1] !== '$') {
        ast = ctx.mkReConcat(ast, ctx.mkReStar(Any()));
        implier = ctx.mkSeqConcat([implier, nextFiller()]);
    }

    //TODO: Fix tagging to be multiline
    return {
        ast: ast.tag('' + regex),
        implier: implier,
        assertions: assertions,
        captures: captures
    };
}

function RegexOuter(ctx, regex) {
    return RegexRecursive(ctx, CullOuterRegex('' + regex), 0, false);
}

export default RegexOuter;