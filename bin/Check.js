"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = _default;

/**
 * Copyright Blake Loring <blake_l@parsed.uk>
 */
function _default(check_predicate, altgen) {
  return function (query, model) {
    //Returns the Query check structure with either a list of alternative queries or nothing
    var sat = check_predicate(model);
    return {
      isSAT: sat,
      alternatives: !sat ? altgen(query, model) : []
    };
  };
}