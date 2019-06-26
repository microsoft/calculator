let isIOS = !!navigator.platform && /iPad|iPhone|iPod/.test(navigator.platform);
let isProduction = window.location.hostname.toLowerCase().indexOf("platform.uno") !== -1;

var parts = window.location.hostname.split('.');

if (isProduction && parts.length > 0) {
    if (isIOS) {
        window.location.href = "https://bit.ly/calc-ios";
    }
}
