/*
    These are just color utility functions only used by this script
*/
function componentToHex(c : number) {
    var hex = c.toString(16);
    return hex.length == 1 ? "0" + hex : hex;
}

export function SetBarProgressColor(r : number, g : number, b : number) {
    document.body.style.setProperty("--bar-progression", "#" + componentToHex(r) + componentToHex(g) + componentToHex(b));
}