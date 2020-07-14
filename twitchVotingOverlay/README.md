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

There is a package called webpack dev server which is currently **NOT** installed (if someone wants to do that, feel free). That's why I recommend using `npx webpack --watch --mode=development`. This will compile the code, and re-compile on changes. If you now open the `index.html`, with a tool that refreshes the webpage on changes, I use [this](https://marketplace.visualstudio.com/items?itemName=ritwickdey.LiveServer) with VS Code, the webpage will automatically refresh and show the changes.

# Basic usage

Use the the `ChaosOverlayClient`:

```typescript
const OVERLAY_CLIENT = new ChaosOverlayClient('ws://localhost:9091');

OVERLAY_CLIENT.addCreateVoteListener(() => /* create vote code*/);
OVERLAY_CLIENT.addUpdateVoteListener(() => /* update vote code */);
OVERLAY_CLIENT.addEndVoteListener(() => /* vote ended code */);
```

With those event you should be able to write your overlay.
