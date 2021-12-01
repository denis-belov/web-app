import '@babel/polyfill';

import WasmWrapper from '../../xgk-js/src/wasm-wrapper.js';



onmessage = async ({ data }) =>
{
	LOG(data)

	const wasm = new WasmWrapper();

	await wasm.init(data.code, data.memory);

	wasm.exports.initTransitionStack();



	// setTimeout(wasm.exports.startTransition, 3000);

	setInterval(wasm.exports.updateTransitions, 0);
};
