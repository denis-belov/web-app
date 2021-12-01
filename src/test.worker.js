import '@babel/polyfill';

import WasmWrapper from '../../xgk-js/src/wasm-wrapper.js';



onmessage = async ({ data }) =>
{
	const wasm = new WasmWrapper();

	await wasm.init(data.code, data.memory);

	// wasm.exports.initTransitionStack();



	setInterval(wasm.exports.updateTransitions);
};
