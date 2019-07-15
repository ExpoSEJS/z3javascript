/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

var _Z = _interopRequireDefault(require("../Z3"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

var ctx = new _Z["default"].Context();
var solver = new _Z["default"].Solver(ctx, false, []);

function Test(Origin) {
  solver.reset();

  var TestRegex = _Z["default"].Regex(ctx, Origin);

  var symbolic = ctx.mkConst(ctx.mkStringSymbol('TestC0'), ctx.mkStringSort()); //Assert to make capture correct all the time solver.assert(ctx.mkEq(TestRegex.captures[1], ctx.mkString('')));
  //solver.assert(ctx.mkEq(TestRegex.captures[1], ctx.mkString('a')));

  solver.assert(ctx.mkSeqInRe(symbolic, TestRegex.ast));
  solver.assert(ctx.mkImplies(ctx.mkSeqInRe(symbolic, TestRegex.ast), ctx.mkEq(symbolic, TestRegex.implier)));
  TestRegex.assertions.forEach(function (assert) {
    solver.assert(assert);
  });

  function Exists(array1, array2, pred) {
    for (var i = 0; i < array1.length; i++) {
      if (pred(array1[i], array2[i])) {
        return true;
      }
    }

    return false;
  }

  function DoesntMatch(l, r) {
    if (l === undefined) {
      return r !== '';
    } else {
      return l !== r;
    }
  }

  function CheckCorrect(model) {
    var real_match = Origin.exec(model.eval(symbolic).asConstant());
    var sym_match = TestRegex.captures.map(function (cap) {
      return model.eval(cap).asConstant();
    });
    var matches = real_match && !Exists(real_match, sym_match, DoesntMatch);
    console.log('Matches:', matches, model.eval(symbolic).asConstant(), Origin);
    return matches;
  }

  var NotMatch = _Z["default"].Check(CheckCorrect, function (query, model) {
    var query_list = query.exprs.concat([ctx.mkNot(ctx.mkEq(symbolic, ctx.mkString(model.eval(symbolic).asConstant())))]);
    return new _Z["default"].Query(query_list, query.checks);
  });

  var CheckFixed = _Z["default"].Check(CheckCorrect, function (query, model) {
    var real_match = Origin.exec(model.eval(symbolic).asConstant());

    if (!real_match) {
      return [];
    } else {
      real_match = Origin.exec(model.eval(symbolic).asConstant()).map(function (match) {
        return match || '';
      });
      console.log("Here ".concat(real_match.length, " in ").concat(TestRegex.captures.length));
      TestRegex.captures.forEach(function (x, idx) {
        console.log("".concat(x, " => ").concat(real_match[idx]));
      });
      var query_list = TestRegex.captures.map(function (cap, idx) {
        return ctx.mkEq(ctx.mkString(real_match[idx]), cap);
      });
      return [new _Z["default"].Query(query.exprs.concat(query_list), [_Z["default"].Check(CheckCorrect, function (query, model) {
        return [];
      })])];
    }
  });

  var query = new _Z["default"].Query([], [CheckFixed, NotMatch]);
  var mdl = query.getModel(solver);

  if (mdl) {
    var match = Origin.exec(mdl.eval(symbolic).asConstant());
    console.log('Modelled Match: ' + mdl.eval(symbolic).asConstant());

    if (match) {
      console.log("Model: ".concat(mdl.eval(symbolic).asConstant(), " Captures: ").concat(JSON.stringify(match)));
      console.log('Match Length: ' + match.length + ' CapturesLength: ' + TestRegex.captures.length);
      return CheckCorrect(mdl) ? 'GOOD' : 'BAD CAPTURE';
    } else {
      return 'BAD MATCH';
    }
  } else {
    return 'UNSAT';
  }
}

var test_re = [/hello/, /[0-9]{3,}/, /[0-9]{undefined}/, /[0-9]{5}/, /(?!hi)hello/, /(?=hello)hello/, /(?=[12345])./, /webkit|android|google/, /(?:webkit)?google/, /^\bGiggles$/, /^(.*)\1(Hello)\2$/, /^([12345]+)\1$/, /^Hello.\bWorld$/, /^<(.+)>.+<\1>$/, /(Capture)\1/, /^\bGiggles\b$/, /^((?!chrome|android).)*safari/i];
var failed = 0;
test_re.forEach(function (re) {
  try {
    console.log('Testing', re);

    if (Test(re) != 'GOOD') {
      throw re;
    }
  } catch (e) {
    failed += 1;
    console.log('Failed', '' + e);
  }
});

if (failed) {
  throw failed + ' errors';
}

module.exports = Test;