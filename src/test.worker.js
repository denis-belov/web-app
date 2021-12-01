import '@babel/polyfill';

import WasmWrapper from '../../xgk-js/src/wasm-wrapper.js';

// import wasm_code from './cpp/src/entry-wasm32.cpp';



onmessage = async ({ data }) =>
{
	LOG(data)

	const wasm = new WasmWrapper();

	await wasm.init(data.code, data.memory);

	// // wasm.exports.setWindowSize(window.innerWidth / 2, window.innerHeight);

	// // wasm.exports.main();

	wasm.exports.initTransitionStack();



	setTimeout(wasm.exports.startTransition, 3000);

	setInterval(wasm.exports.updateTransitions, 100);
};
