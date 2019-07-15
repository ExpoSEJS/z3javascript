"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

function _slicedToArray(arr, i) { return _arrayWithHoles(arr) || _iterableToArrayLimit(arr, i) || _nonIterableRest(); }

function _nonIterableRest() { throw new TypeError("Invalid attempt to destructure non-iterable instance"); }

function _iterableToArrayLimit(arr, i) { var _arr = []; var _n = true; var _d = false; var _e = undefined; try { for (var _i = arr[Symbol.iterator](), _s; !(_n = (_s = _i.next()).done); _n = true) { _arr.push(_s.value); if (i && _arr.length === i) break; } } catch (err) { _d = true; _e = err; } finally { try { if (!_n && _i["return"] != null) _i["return"](); } finally { if (_d) throw _e; } } return _arr; }

function _arrayWithHoles(arr) { if (Array.isArray(arr)) return arr; }

/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 * Approximate JavaScript regular expression to Z3 regex parser
 */
function CullOuterRegex(regex) {
  var firstSlash = regex.indexOf("/");
  var lastSlash = regex.lastIndexOf("/");
  return regex.substr(firstSlash + 1, lastSlash - 1);
}

function FindClosingParen(regex, idx) {
  var open = 1;

  while (idx < regex.length) {
    if (regex[idx - 1] != "\\" && regex[idx] == "(") {
      open++;
    }

    if (regex[idx - 1] != "\\" && regex[idx] == ")") {
      open--;
    }

    if (open == 0) {
      return idx;
    }

    idx++;
  }

  return -1;
}

function Desugar(regex) {
  var i; //Strip ?!

  while ((i = regex.indexOf("(?!")) != -1 || (i = regex.indexOf("?=")) != -1) {
    var end = FindClosingParen(regex, i + 1);
    regex = regex.slice(0, i) + regex.slice(end + 1);
  }

  while ((i = regex.indexOf("(")) != -1 || (i = regex.indexOf(")")) != -1) {
    if (regex[i - 1] != "\\") {
      regex = regex.slice(0, i) + regex.slice(i + 1);
    }
  } //Remove word boundaries


  regex = regex.replace(/\\b|\\B/g, "");
  return regex;
}

var REGEX_CTR = 0;

function RegexRecursive(ctx, regex, idx) {
  var pp_steps = [];
  var lrctr = REGEX_CTR++;
  var captures = [];
  var previousCaptureAst = [];
  var assertions = [];
  var fill_ctr = 0;
  var backreferences = false;

  function BuildError(msg) {
    return {
      error: msg,
      idx: idx,
      remaining: regex.slice(idx)
    };
  } //TODO: This is a bad way of handling symbolIn, in general the whole processing fillers is weak


  var shouldAddFillerIn = true;

  function nextFiller() {
    return ctx.mkStringVar("" + lrctr + " Fill " + fill_ctr++);
  }

  function moreRange() {
    return idx < regex.length;
  }

  function more() {
    return moreRange() && current() != "|" && current() != ")";
  }

  function mk(v) {
    return ctx.mkSeqToRe(ctx.mkString(v));
  }

  function current() {
    if (regex.length > idx) {
      return regex[idx];
    }

    return undefined;
  }

  function next(num) {
    var r = current();
    idx += num || 1;
    return r;
  }

  function peek(num) {
    if (typeof num === "undefined") {
      num = 1;
    }

    return regex[idx + num];
  }

  function Any() {
    var beforeNewline = ctx.mkReRange(ctx.mkString("\\x00"), ctx.mkString("\\x09"));
    var afterNewline = ctx.mkReRange(ctx.mkString("\\x0b"), ctx.mkString("\\xff"));
    return ctx.mkReUnion(beforeNewline, afterNewline);
  }
  /**
      * The . character isnt all chracters
      * This will accept any character
      */


  function TruelyAny() {
    return ctx.mkReRange(ctx.mkString("\\x00"), ctx.mkString("\\xff"));
  }

  function ParseRangerNextEscaped() {
    var c1 = next();

    if (c1 == "\\") {
      return next();
    } else {
      return c1;
    }
  }

  function ParseRangeInner() {
    var union = undefined;

    while (moreRange() && current() != "]") {
      var c1 = ParseRangerNextEscaped();
      var range = undefined;

      if (current() == "-" && peek() != "]") {
        next();
        var c2 = ParseRangerNextEscaped();
        range = ctx.mkReRange(ctx.mkString(c1), ctx.mkString(c2));
      } else {
        range = ctx.mkSeqToRe(ctx.mkString(c1));
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
    var negate = false;

    if (current() == "^") {
      next();
      negate = true;
    }

    var r = ParseRangeInner();

    if (negate) {
      var comp = ctx.mkReComplement(r);
      r = ctx.mkReIntersect(TruelyAny(), comp);
    }

    if (next() == "]") {
      return r;
    } else {
      throw BuildError("Regex Parsing Error (Range)");
    }
  }

  var Specials = {
    ".": Any
  };

  function Alpha() {
    var p1 = ctx.mkReRange(ctx.mkString("a"), ctx.mkString("z"));
    var p2 = ctx.mkReRange(ctx.mkString("A"), ctx.mkString("Z"));
    return ctx.mkReUnion(p1, p2);
  }

  function Digit() {
    return ctx.mkReRange(ctx.mkString("0"), ctx.mkString("9"));
  }

  function Whitespace() {
    var p1 = mk(" ");
    var p2 = mk("\t");
    var p3 = mk("\r");
    var p4 = mk("\n");
    var p5 = mk("\f");
    var p6 = mk("\v");
    return ctx.mkReUnion(p1, ctx.mkReUnion(p2, ctx.mkReUnion(p3, ctx.mkReUnion(p4, ctx.mkReUnion(p5, p6)))));
  }

  function AlphaNumeric() {
    return ctx.mkReUnion(Alpha(), Digit());
  }

  function Word() {
    return ctx.mkReUnion(AlphaNumeric(), mk("_"));
  }

  function ParseAtom1() {
    var parsed_str = next();
    var IS_JUST_TEXT = /^[a-zA-Z0-9]$/;
    var IS_SPECIAL = /^[*+?]$/; //Hack to greedly eat anything that is definately not a special character
    //Makes SMT formulee look prettier
    //We need to look ahead for this and just drop back to standard parsing atom by atom if
    //the lookahead is special

    while (current() && IS_JUST_TEXT.test(current()) && !IS_SPECIAL.test(peek())) {
      parsed_str += next();
    }

    return mk(parsed_str);
  }

  function ParseMaybeSpecial() {
    if (Specials[current()]) {
      return Specials[next()]();
    } else {
      return ParseAtom1();
    }
  }

  function isHex(_char) {
    return /^[0-9A-Fa-f]+$/.test(_char);
  }
  /*
  function isDigits(char) {
  	return /^[0-9]+$/.test(char);
  }
   */


  function ParseMaybeEscaped(captureIndex) {
    if (current() == "\\") {
      next();
      var c = next();

      if (c == "d") {
        return Digit();
      } else if (c == "D") {
        return ctx.mkReIntersect(TruelyAny(), ctx.mkReComplement(Digit()));
      } else if (c == "w") {
        return Word();
      } else if (c == "W") {
        return ctx.mkReIntersect(TruelyAny(), ctx.mkReComplement(Word()));
      } else if (c == "s") {
        return Whitespace();
      } else if (c == "S") {
        return ctx.mkReIntersect(TruelyAny(), ctx.mkReComplement(Whitespace()));
      } else if (c == "n") {
        return mk("\n");
      } else if (c == "x") {
        var c1 = next();
        var c2 = next();

        if (!isHex(c1) || !isHex(c2)) {
          throw BuildError("Expected hex character at " + c1 + " and " + c2);
        }

        var hexToInt = parseInt(c1 + c2, 16);
        var hexAsString = String.fromCharCode(hexToInt);
        return mk(hexAsString);
      } else if (c == "u") {
        var expectingRBrace = false;

        if (current() == "{") {
          expectingRBrace = true;
          next();
        }

        var unicodeSequence = next() + next() + next() + next();

        if (expectingRBrace && next() != "}") {
          throw BuildError("Expecting RBrace in unicode sequence");
        }

        if (!isHex(unicodeSequence)) {
          throw BuildError("Expected digits in unicode sequence " + unicodeSequence);
        }

        var unicodeString = String.fromCharCode(parseInt(unicodeSequence, 16));
        return mk(unicodeString);
      } else if (c == "r") {
        return mk("\r");
      } else if (c == "v") {
        return mk("\v");
      } else if (c == "t") {
        return mk("\t");
      } else if (c == "f") {
        return mk("\f");
      } else if (c >= "1" && c <= "9") {
        var _idx = parseInt(c);

        if (_idx < captures.length) {
          backreferences = true;
          addToCapture(captureIndex, captures[_idx]);
          shouldAddFillerIn = false;
          return previousCaptureAst[_idx];
        } else {
          return mk("");
        }
      } else if (c == "b") {
        pp_steps.push({
          type: "b",
          idx: idx
        });
        return mk("");
      } else if (c == "B") {
        pp_steps.push({
          type: "B",
          idx: idx
        });
        return mk("");
      } else if (c == "0") {
        return mk("\\x00");
      }

      return mk(c);
    } else {
      return ParseMaybeSpecial();
    }
  }

  function ParseMaybeRange(captureIndex) {
    if (current() == "[") {
      return ParseRange();
    } else {
      return ParseMaybeEscaped(captureIndex);
    }
  }

  function rewriteCaptureOptional(idx) {
    //Rewrite capture[n] to be capture[n] or ''
    var orFiller = nextFiller();
    either(orFiller, captures[idx], ctx.mkString(""));
    captures[idx] = orFiller;
  }

  function addToCapture(idx, thing) {
    captures[idx] = ctx.mkSeqConcat([captures[idx], thing]);
  }

  function symbolIn(atoms) {
    var nfil = nextFiller();
    assertions.push(ctx.mkSeqInRe(nfil, atoms));
    return nfil;
  }

  function ParseMaybeCaptureGroupStart(captureIndex) {
    function buildPlusConstraints(atoms, plusGroup) {
      var ncap = captures[plusGroup];
      atoms = ctx.mkRePlus(atoms); //String = Something + Capture ^ in atoms

      var added = ctx.mkSeqConcat([nextFiller(), ncap]);
      assertions.push(ctx.mkSeqInRe(added, atoms));
      addToCapture(captureIndex, added);
      return atoms;
    }

    function buildStarConstraints(atoms, starGroup) {
      var ncap = captures[starGroup];
      atoms = ctx.mkReStar(atoms);
      var added = ctx.mkSeqConcat([nextFiller(), ncap]);
      assertions.push(ctx.mkImplies(ctx.mkEq(ncap, ctx.mkString("")), ctx.mkEq(added, ctx.mkString(""))));
      addToCapture(captureIndex, added);
      return atoms;
    }
    /*
      function buildOptionConstraints(atoms, optionGroup) {
    	addToCapture(captureIndex, captures[optionGroup]);
    }
      */


    if (current() == "(") {
      next();
      var capture = true; //Ignore ?: capture groups can't be modelled

      if (current() == "?") {
        next();

        if (current() != ":") {
          throw BuildError("Expected : after ?");
        }

        next();
        capture = false;
      }

      var newestCapture = captures.length;
      var atoms = ParseCaptureGroup(captureIndex, capture);

      if (next() != ")") {
        throw BuildError("Expected ) (Capture Group Close)");
      }

      if (capture) {
        switch (current()) {
          case "?":
          case "*":
            {
              //If anything the capture is optional then anything inside it is also optional
              //TODO: Take a list of originals and rewrite an implication
              //iff Len(origin) > 0 then c[i] = o[i]
              for (var i = newestCapture; i < captures.length; i++) {
                rewriteCaptureOptional(i);
              }

              buildStarConstraints(atoms, newestCapture);
              break;
            }

          case "{":
          case "+":
            {
              buildPlusConstraints(atoms, newestCapture);
              break;
            }

          default:
            {
              addToCapture(captureIndex, captures[newestCapture]);
              break;
            }
        }
      }

      shouldAddFillerIn = false;
      return atoms;
    } else {
      return ParseMaybeRange(captureIndex);
    }
  }

  function ParseMaybeAssertion(captureIndex) {
    if (current() == "(" && peek() == "?" && (peek(2) == "!" || peek(2) == "=")) {
      var end = FindClosingParen(regex, idx + 3);
      var re = regex.slice(idx + 3, end);
      pp_steps.push({
        type: peek(2),
        re: re,
        idx: end + 1
      });
      idx = end + 1;
    }

    return ParseMaybeCaptureGroupStart(captureIndex);
  }

  function ParseMaybePSQ(captureIndex) {
    var atom = ParseMaybeAssertion(captureIndex);

    if (current() == "*") {
      next();

      if (current() == "?") {
        next();
      }

      atom = ctx.mkReStar(atom);
    } else if (current() == "+") {
      next();

      if (current() == "?") {
        next();
      }

      atom = ctx.mkRePlus(atom);
    } else if (current() == "?") {
      next();

      if (current() == "?") {
        next();
      }

      atom = ctx.mkReOption(atom);
    }

    return atom;
  }

  function digit(offset) {
    if (typeof offset === "undefined") {
      offset = 0;
    }

    return peek(offset) >= "0" && peek(offset) <= "9";
  }

  function ParseNumber() {
    var numStr = "";

    if (!digit()) {
      throw BuildError("Expected Digit (Parse Number)");
    }

    while (digit()) {
      numStr += next();
    }

    return parseInt(numStr);
  }

  function ParseLoopCount() {
    var n1 = ParseNumber();

    if (current() == ",") {
      next();

      if (!digit()) {
        //Either a syntax error or a min loop, assume a min loop
        return [n1, undefined];
      } else {
        var n2 = ParseNumber();
        return [n1, n2];
      }
    } else {
      return [n1, n1];
    }
  }

  function ParseMaybeLoop(captureIndex) {
    var atom = ParseMaybePSQ(captureIndex);

    if (current() == "{" && digit(1)) {
      next();

      var _ParseLoopCount = ParseLoopCount(),
          _ParseLoopCount2 = _slicedToArray(_ParseLoopCount, 2),
          lo = _ParseLoopCount2[0],
          hi = _ParseLoopCount2[1];

      if (!next() == "}") {
        throw BuildError("Expected } following loop count");
      } //Discard any succeeding ?


      if (current() == "?") {
        next();
      } //If hi is undefined then it's a min loop {5,}


      if (hi === undefined) {
        atom = ctx.mkReConcat(ctx.mkReLoop(atom, lo, lo), ctx.mkReStar(atom));
      } else {
        atom = ctx.mkReLoop(atom, lo, hi);
      }
    }

    return atom;
  }

  function ParseMaybeAtoms(captureIndex) {
    var rollup = null;

    while (more()) {
      while (current() == "^" || current() == "$") {
        //TODO: Find out how to handle multiline
        next();
      } //TODO: This is horrible, anchors should be better


      if (more()) {
        //let capturesStart = captures.length;
        var parsed = ParseMaybeLoop(captureIndex);

        if (shouldAddFillerIn) {
          addToCapture(captureIndex, symbolIn(parsed));
        }

        shouldAddFillerIn = true;
        rollup = rollup ? ctx.mkReConcat(rollup, parsed) : parsed;
      }
    }

    return rollup || mk("");
  }

  function either(v, left, right) {
    assertions.push(ctx.mkOr(ctx.mkEq(v, left), ctx.mkEq(v, right)));
    return v;
  }

  function buildAlternationCaptureConstraints(captureIndex, startCaptures, endLeftCaptures, endRightCaptures, cLeft, cRight) {
    var leftCaptures = [];
    var leftOriginals = [];
    var rightCaptures = [];
    var rightOriginals = [];

    for (var i = startCaptures; i < endLeftCaptures; i++) {
      leftOriginals.push(captures[i]);
      rewriteCaptureOptional(i);
      leftCaptures.push(captures[i]);
    }

    for (var _i2 = endLeftCaptures; _i2 < endRightCaptures; _i2++) {
      rightOriginals.push(captures[_i2]);
      rewriteCaptureOptional(_i2);
      rightCaptures.push(captures[_i2]);
    }

    var cFinal = nextFiller();

    function buildSide(side, left, original, right) {
      var forceRightNothing = right.map(function (x) {
        return ctx.mkEq(x, ctx.mkString(""));
      });
      var forceLeftOriginal = left.map(function (x, idx) {
        return ctx.mkEq(left[idx], original[idx]);
      });
      assertions.push(ctx.mkImplies(ctx.mkEq(cFinal, side), ctx.mkAndList(forceRightNothing.concat(forceLeftOriginal))));
    }

    buildSide(cLeft, leftCaptures, leftOriginals, rightCaptures);
    buildSide(cRight, rightCaptures, rightOriginals, leftCaptures);
    either(cFinal, cLeft, cRight);
    captures[captureIndex] = cFinal;
  }

  function ParseMaybeOption(captureIndex) {
    //Track the length of captures through parsing of either side
    //If it changes then the blocks parsed have a capture in them
    //and will need extra constraints
    var startCaptures = captures.length; //The captures on an option are a bit tricky
    //The capture is either going to be the current C0 + [Some stuff] | [Some Stuff]
    //So we parse one side, reset the capture to cStart, then parse the other and express
    //the final constraint as an or of the two

    var cStart = captures[captureIndex];
    captures[captureIndex] = ctx.mkString("");
    var ast = ParseMaybeAtoms(captureIndex); //Track the end of the left captures

    var endLeftCaptures = captures.length;
    var cLeft = captures[captureIndex];

    if (current() == "|") {
      captures[captureIndex] = ctx.mkString("");
      next();
      var ast2 = ParseMaybeOption(captureIndex);
      var cRight = captures[captureIndex];
      var endRightCaptures = captures.length; //If any capture groups have been defined in the alternation we need to
      //build some new string constraints on the result

      buildAlternationCaptureConstraints(captureIndex, startCaptures, endLeftCaptures, endRightCaptures, ctx.mkSeqConcat([cStart, cLeft]), ctx.mkSeqConcat([cStart, cRight]));
      ast = ctx.mkReUnion(ast, ast2);
    } else {
      captures[captureIndex] = ctx.mkSeqConcat([cStart, cLeft]);
    }

    return ast;
  }

  function ParseCaptureGroup(captureIndex, capture) {
    if (capture) {
      captureIndex = captures.length;
      previousCaptureAst.push(null);
      captures.push(ctx.mkString(""));
    }

    var r = ParseMaybeOption(captureIndex);

    if (capture) {
      assertions.push(ctx.mkSeqInRe(captures[captureIndex], r));
      previousCaptureAst[captureIndex] = r;
    }

    return r;
  }

  var ast = ParseCaptureGroup(0, true);
  var implier = captures[0];
  var startIndex;
  var anchoredStart = undefined;
  var anchoredEnd = undefined;

  if (regex[0] !== "^") {
    anchoredStart = nextFiller();
    ast = ctx.mkReConcat(ctx.mkReStar(TruelyAny()), ast);
    implier = ctx.mkSeqConcat([anchoredStart, implier]);
    startIndex = ctx.mkSeqLength(anchoredStart);
  } else {
    startIndex = ctx.mkIntVal(0);
  }

  if (regex[regex.length - 1] !== "$") {
    anchoredEnd = nextFiller();
    ast = ctx.mkReConcat(ast, ctx.mkReStar(TruelyAny()));
    implier = ctx.mkSeqConcat([implier, anchoredEnd]);
  }
  /**
      * All assertions are post-processed into SMT
      * This is done be re-parsing a desugared version of the regex and intersecting it with the AST
      */


  pp_steps.forEach(function (item) {
    var ds_lhs = Desugar(regex.substr(0, item.idx));
    var ds_rhs = Desugar(regex.substr(item.idx));
    var lhs = RegexRecursive(ctx, ds_lhs + "$", 0).ast;
    var rhs = RegexRecursive(ctx, "^" + ds_rhs, 0).ast;

    if (item.type == "b" || item.type == "B") {
      //Constants for use in query
      var any_string = ctx.mkReStar(TruelyAny());
      var word = Word();
      var not_word = ctx.mkReComplement(Word()); //If B then flip word and not word

      if (item.type == "B") {
        var t = not_word;
        not_word = word;
        word = t;
      }

      var empty_string = mk(""); //L = lhs in .*\W & rhs in \w.*

      var l1_w = ctx.mkReConcat(any_string, not_word);
      var l1 = ctx.mkReUnion(ctx.mkReIntersect(lhs, l1_w), empty_string);
      var l2_w = ctx.mkReConcat(word, any_string);
      var l2 = ctx.mkReIntersect(rhs, l2_w);
      var l = ctx.mkReConcat(l1, l2); //R - lhs in .*\w & rhs in \W.*

      var r1_w = ctx.mkReConcat(any_string, word);
      var r1 = ctx.mkReIntersect(lhs, r1_w);
      var r2_w = ctx.mkReConcat(not_word, any_string);
      var r2 = ctx.mkReUnion(ctx.mkReIntersect(rhs, r2_w), empty_string);
      var r = ctx.mkReConcat(r1, r2); //Assert ast intersects l | r

      ast = ctx.mkReIntersect(ast, ctx.mkReUnion(l, r));
    } else if (item.type == "=" || item.type == "!") {
      //Compute the asserted regex
      var assert = RegexRecursive(ctx, "^" + item.re + "$", 0).ast;
      assert = ctx.mkReConcat(assert, ctx.mkReStar(TruelyAny())); //Negate it if ?!

      if (item.type == "!") {
        assert = ctx.mkReComplement(assert);
      }

      var lr = ctx.mkReConcat(lhs, ctx.mkReIntersect(rhs, assert));
      ast = ctx.mkReIntersect(ast, lr);
    } else {
      throw "Currently unsupported";
    }
  }); //Give unique names to all captures for Ronny

  for (var i = 0; i < captures.length; i++) {
    var _current = captures[i];
    captures[i] = nextFiller();
    assertions.push(ctx.mkEq(captures[i], _current));
  } //TODO: Fix tagging to be multiline


  return {
    ast: ast,
    implier: implier,
    assertions: assertions,
    captures: captures,
    startIndex: startIndex,
    anchoredStart: anchoredStart,
    anchoredEnd: anchoredEnd,
    backreferences: backreferences,
    idx: idx //Return the index so recursion assertions work out

  };
}

function RegexOuter(ctx, regex) {
  try {
    return RegexRecursive(ctx, CullOuterRegex("" + regex), 0, false);
  } catch (e) {
    throw "".concat(e.error ? e.error.toString() : "" + e, " ").concat(e.idx, " \"").concat(e.remaining, "\" parsing regex \"").concat(regex, "\" ").concat(e.stack);
  }
}

var _default = RegexOuter;
exports["default"] = _default;