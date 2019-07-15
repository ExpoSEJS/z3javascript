/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
const Z3Loader = require("./package");
const Z3Path = process && process.env.Z3_PATH ? process.env.Z3_PATH : undefined;
export default Z3Loader(Z3Path);
