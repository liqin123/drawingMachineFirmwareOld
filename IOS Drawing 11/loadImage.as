package {

	import flash.display.Sprite;
	import flash.utils.ByteArray;
	import flash.display.Loader;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.net.URLLoaderDataFormat;
	import flash.geom.Rectangle;
	import flash.events.Event;

	public class loadImage extends Sprite {

		private var urlLoader: URLLoader = new URLLoader();

		public function loadImage() {

		}

		public function loadUrl(str: String) {
			trace("loadUrl");
			urlLoader.dataFormat = URLLoaderDataFormat.BINARY;
			urlLoader.load(new URLRequest(str));

			urlLoader.addEventListener(Event.COMPLETE, completeHandler);
		}

		private function completeHandler(event: Event): void {
			trace("loadUrl callback");
			var loader: Loader = new Loader();
			loader.loadBytes(urlLoader.data);
			addChild(loader);
		}
		public function fileExistsServer(fileName: String) {

			var fileExists: FileExistsUtil = new FileExistsUtil();
			
			fileExists.checkFile("https:s3-us-west-2.amazonaws.com/drawingmachine/" + fileName, function (eventType: String): void {
					trace("exists");
					trace(eventType);
				},
				function (errorType: String, text: String): void {
					//trace("not found");
					//trace(errorType + ": " + text);
				});
		}


	}
}