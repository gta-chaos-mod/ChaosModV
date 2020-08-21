const path = require('path');

module.exports = {
	entry: './src/index.ts',
	devtool: 'inline-source-map',
	devServer: {
		port: 9090,
		contentBase: path.join(__dirname, 'dist'),
		watchContentBase: true,
		publicPath: '/',
	},

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
		extensions: ['.ts', '.js'],
	},
	output: {
		filename: 'bundle.js',
		path: path.resolve(__dirname, 'dist'),
	},
};
