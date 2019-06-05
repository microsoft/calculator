namespace Uno.UI.Demo {
	export class Analytics {
		private static isLoaded: boolean = false;

		public static reportPageView(screenName: string, appName: string = "calculator-wasm"): string {

			if (Analytics.init(screenName, appName)) {
				return "ok";
			}

			const gtag = (window as any).gtag;
			if (gtag) {
				gtag("event",
                    'screen', {
                        'event_category': 'User Actions',
                        'event_label': screenName
                    });
			} else {
				console.error(`Google Analytics not present, can't report page view for ${screenName}.`);
			}

			return "ok";
		}

		private static init(screenName: string, appName: string): boolean {
			if (Analytics.isLoaded) {
				return false;
			}

			const script = `
	            window.dataLayer = window.dataLayer || [];
	            function gtag() { dataLayer.push(arguments); }
	            gtag('js', new Date());
	            gtag('config', 'UA-26688675-16');

	            gtag("event", "screen_view", {screen_name: \"${screenName}\", app_name: \"${appName}\"});`;


			const script1 = document.createElement("script");
			script1.type = "text/javascript";
			script1.src = "https://www.googletagmanager.com/gtag/js?id=UA-26688675-16";
			document.body.appendChild(script1);
			const script2 = document.createElement("script");
			script2.type = "text/javascript";
			script2.innerText = script;
			document.body.appendChild(script2);

			Analytics.isLoaded = true;
			return true;
		}
	}
}
