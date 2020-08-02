# Compiling the code

## Prerequisites

- NodeJS

## Installing the packages

Run the command `npm install` **IN THE ROOT OF THE PROJECT**, this is the folder containing the `package.json` and should be in the same folder as this file.

## Compiling the code

This project uses [webpack](https://webpack.js.org/) with the [ts-loader](https://webpack.js.org/guides/typescript/) to transpile the typescript source into regular javascript.

To compile the code you can either use `npm run compile:dev` or `npm run compile:prod`. The differences can be seen [here](https://webpack.js.org/configuration/mode/).

After that it's just a matter of opening the `dist/index.html` file.

## Developing

This project uses [webpack](https://webpack.js.org/). To automatically set up a server which will recompile and reload the page, run `npm run develop` in a terminal. Note that these compiled files will only be served from `webpack-dev-server`, and you'll need to go through the normal compile steps for production use.

# Basic usage

Use the the `ChaosOverlayClient`:

```typescript
const OVERLAY_CLIENT = new ChaosOverlayClient('ws://localhost:9091');

OVERLAY_CLIENT.addCreateVoteListener(() => /* create vote code*/);
OVERLAY_CLIENT.addUpdateVoteListener(() => /* update vote code */);
OVERLAY_CLIENT.addEndVoteListener(() => /* vote ended code */);
```

With those event you should be able to write your overlay.
