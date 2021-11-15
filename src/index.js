import './index.scss';
import '@babel/polyfill';

import WasmWrapper from '../../xgk-js/src/wasm-wrapper.js';
import WebGLRenderer from '../../xgk-js/src/webgl-renderer.js';

import wasm_code from './cpp/src/entry-wasm32.cpp';



window.addEventListener
(
	'load',

	async () =>
	{
		const wasm = new WasmWrapper();

		await wasm.init(wasm_code);

		wasm.exports.main();

		const webgl_renderer = new WebGLRenderer(wasm, document.querySelector('canvas'));

		const gl = webgl_renderer._context;

		const scene = new webgl_renderer.Scene(wasm.Addr(wasm.exports.scene.value));
		const material = new webgl_renderer.Material(wasm.Addr(wasm.exports.material.value));
		const _object = new webgl_renderer.Object(wasm.Addr(wasm.exports.object.value));

		LOG(scene);
		LOG(material);
		LOG(_object);



		const b = gl.createBuffer();

		gl.bindBuffer(gl.ARRAY_BUFFER, b);
		gl.bufferData(gl.ARRAY_BUFFER, scene.vertex_data, gl.STATIC_DRAW);
		gl.vertexAttribPointer(0, 3, gl.FLOAT, 0, 0, 0);

		gl.enableVertexAttribArray(0);



		const render = () =>
		{
			gl.clear(gl.COLOR_BUFFER_BIT);

			gl.useProgram(material.program);

			gl.drawArrays(material.topology, 0, 9);

			requestAnimationFrame(render);
		};

		render();
	},
);
