/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 * Approximate JavaScript regular expression to Z3 regex parser
 */

function CullOuterRegex(regex) {
    let firstSlash = regex.indexOf('/');
    let lastSlash = regex.lastIndexOf('/');
    return regex.substr(firstSlash + 1, lastSlash - 1);
}

function RegexRecursive(ctx, regex, idx) {

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
            if (!range) { return null; }
            return range;
        } else {
            return ParseMaybeEscaped();
        }
    }

    function ParseMaybeSubRegex() {
        if (current() == '(') {
            next();
            
            let atoms = ParseMaybeOption();
            
            if (next() != ')') {
                return null;
            }

            return atoms;
        } else {
            return ParseMaybeRange();
        }
    }

    function ParseMaybePSQ() {
        
        let atom = ParseMaybeSubRegex();

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

    function ParseMaybeLoop() {
        let atom = ParseMaybePSQ();

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

    function ParseMaybeAtoms() {

        let rollup = null;

        while (more()) {

            while (current() == '^' || current() == '$') {
                //TODO: Find out how to handle multiline
                next();
            }

            //TODO: This is horrible, anchors should be better
            if (more()) {
            	let parsed = ParseMaybeLoop();
            	
            	if (!parsed) {
            		return null;
            	}

                rollup = rollup ? ctx.mkReConcat(rollup, parsed) : parsed;
            }
        }

        return rollup.simplify();
    }

    function ParseMaybeOption() {
        let ast = ParseMaybeAtoms();

        if (!ast) {
            return null;
        }

        if (current() == '|') {
            next();

            let ast2 = ParseMaybeOption();

            if (!ast2) {
                return null;
            }

            return ctx.mkReUnion(ast, ast2);
        } else {
            return ast;
        }
    }

    let ast = ParseMaybeOption();

    if (regex[0] !== '^') {
        ast = ctx.mkReConcat(ctx.mkReStar(Any()), ast);
    }

    if (regex[regex.length - 1] !== '$') {
        ast = ctx.mkReConcat(ast, ctx.mkReStar(Any()));
    }

    return ast;
}

function RegexOuter(ctx, regex) {
    return RegexRecursive(ctx, CullOuterRegex('' + regex), 0, false).tag('' + regex);
}

export default RegexOuter;