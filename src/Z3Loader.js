import Z3Loader from './package';
let Z3Path = process && process.env.Z3_PATH ? process.env.Z3_PATH : undefined;
export default Z3Loader(Z3Path);