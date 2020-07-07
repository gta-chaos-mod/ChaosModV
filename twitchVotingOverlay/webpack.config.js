const path = require('path');

module.exports = {
	entry: './src/index.ts',
	devtool: 'inline-source-map',
	module: {
		rules: [
			{
				exclude: /node_modules/,
				test: /\.ts/,
				use: 'ts-loader',
			},
		],
	},
	resolve: {
		extensions: ['.ts'],
	},
	output: {
		filename: 'bundle.js',
		path: path.resolve(__dirname, 'dist'),
	},
};
