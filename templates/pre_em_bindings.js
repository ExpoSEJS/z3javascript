"use strict";

import Module from './z3.emscripten.js';

// Manually defined types (from Z3 Python API). Could possibly be simplified to just Voidp
// but maybe we'll need the distinction later
let Void  = null,
    Voidp = 'number';

let Z3Exception = Voidp;
let ContextObj = Voidp;
let TheoryObj = Voidp;
let Config = Voidp;
let Symbol = Voidp;
let Sort = Voidp;
let FuncDecl = Voidp;
let Ast = Voidp;
let Pattern = Voidp;
let Model = Voidp;
let Literals = Voidp;
let Constructor = Voidp;
let ConstructorList = Voidp;
let GoalObj = Voidp;
let TacticObj = Voidp;
let ProbeObj = Voidp;
let ApplyResultObj = Voidp;
let StatsObj = Voidp;
let SolverObj = Voidp;
let FixedpointObj = Voidp;
let ModelObj = Voidp;
let AstVectorObj = Voidp;
let AstMapObj = Voidp;
let Params = Voidp;
let ParamDescrs = Voidp;
let FuncInterpObj = Voidp;
let FuncEntryObj = Voidp;
let RCFNumObj = Voidp;

// Names for standard types
let CUInt = 'number';
let CInt = 'number';
let CBool = 'number';
let CULong = 'number';
let CLong = 'number';
let CDouble = 'number';
let CString = 'string';

// Array types. Not all of these may be valid (check over time)
let AstArray = 'array';
let CUIntArray = 'array';
let SymbolArray = 'array';
let SortArray = 'array';
let FuncDeclArray = 'array';
let ConstructorArray = 'array';
let ConstructorListArray = 'array';
let PatternArray = 'array';
let TacticObjArray = 'array';
let RCFNumObjArray = 'array';