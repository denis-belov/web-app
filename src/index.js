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

		LOG(wasm.Floatv(wasm.Addr(wasm.exports.orbit.value), 16, 16))

		const scene = new webgl_renderer.Scene(wasm.Addr(wasm.exports.scene.value));
		const material = new webgl_renderer.Material(wasm.Addr(wasm.exports.material.value));
		const material2 = new webgl_renderer.Material(wasm.Addr(wasm.exports.material2.value));
		const _object = new webgl_renderer.Object(wasm.Addr(wasm.exports.object.value));
		const object2 = new webgl_renderer.Object(wasm.Addr(wasm.exports.object2.value));
		const orbit = wasm.Addr(wasm.exports.orbit.value);

		LOG(scene);
		LOG(material);
		LOG(material2);
		LOG(_object);
		LOG(object2);



		const b = gl.createBuffer();

		gl.bindBuffer(gl.ARRAY_BUFFER, b);
		gl.bufferData(gl.ARRAY_BUFFER, scene.vertex_data, gl.STATIC_DRAW);
		gl.vertexAttribPointer(0, 3, gl.FLOAT, 0, 0, 0);

		gl.enableVertexAttribArray(0);



		const render = () =>
		{
			gl.clear(gl.COLOR_BUFFER_BIT);

			material.use();

			_object.draw();

			material2.use();

			object2.draw();

			requestAnimationFrame(render);
		};

		render();



		setInterval
		(
			() =>
			{
				// wasm.exports._ZN3XGK4MATH5Orbit6rotateEv();
				wasm.exports._ZN3XGK4MATH5Orbit7rotate2Eff(orbit, 0.01, 0.01);
				wasm.exports._ZN3XGK4MATH5Orbit6updateEv(orbit);
			},

			0,
		);

		// setTimeout(wasm.exports.destroy, 5000);
	},
);
