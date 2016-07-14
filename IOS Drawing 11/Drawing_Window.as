package {

	import flash.utils.*;
	import flash.display.*;
	import flash.net.*;
	import flash.media.*;
	import flash.system.*;
	import flash.events.*;
	import flash.filters.*;
	import flash.geom.Point;
	public class Drawing_Window extends MovieClip {
		// this is the drawing window and its UI
		var mainMC: MovieClip;
		var fileIO: saving;
		var drawing: Drawing;
		var Drawing_Window_MouseMode_UI_MC: Drawing_Window_MouseMode_UI;

		var Drawing_Window_UI_MC: Drawing_Window_UI;
		var drawControl: DrawControl;

		var imageControl: ImageControl;

		var backgroundImageUI: BackGroundImageUI;

		var webImage: loadImage;

		var _camera: CameraClass;
		var currentBack: String = "";
		var currentBack_visibility: Boolean = true;
		public function Drawing_Window() {

		}

		function getCurrentBackName():String{
			return (currentBack);
		}
		function swapBackImage() {
			if (currentBack == "camera") {
				setBackImage("web");
			} else {
				setBackImage("camera");
			}
		}

		function setBackImage(imageType: String) {
			currentBack = imageType;
			var behindThis: Number;
			switch (imageType) {
				case "camera":
					if (this.webImage) {
						this.removeChild(webImage);
						webImage = null;
					}

					_camera = new CameraClass();
					behindThis = this.getChildIndex(drawing);
					this.addChildAt(_camera, behindThis);
					_camera.alpha = 0.5;
					changeVisibility(true);
					break;

				case "web":
					if (this._camera) {
						this.removeChild(_camera);
						_camera = null;
					}
					webImage = new loadImage();
					behindThis = this.getChildIndex(drawing);
					this.addChildAt(webImage, behindThis);
					webImage.loadUrl("http://marciatest1.weebly.com/uploads/1/4/7/3/14733758/4655472.jpg");
					webImage.alpha = 0.5;
					changeVisibility(true);
					break;
			}
		}

		


		function moveBack(offx, offy) {

			switch (currentBack) {
				case "camera":
					_camera.x += offx;
					_camera.y += offy;
					break;
				case "web":
					webImage.x += offx;
					webImage.y += offy;

					break;
			}

		}

		function scaleImage(_scale: Number) {
			switch (currentBack) {
				case "camera":

					_camera.scaleX = _scale;
					_camera.scaleY = _scale;
					break;

				case "web":
					webImage.scaleX = _scale;
					webImage.scaleY = _scale;
					break;
			}
		}

		function changeVisibility(state: Boolean) {
			switch (currentBack) {
				case "camera":
					_camera.visible = state;
					currentBack_visibility = state;
					break;

				case "web":
					webImage.visible = state;
					currentBack_visibility = state;
					break;
			}
		}

		public function setup(MainMC) {
			mainMC = MainMC;
			fileIO = new saving(mainMC);


			//setBackImage("camera");

			drawing = new Drawing();
			this.addChild(drawing);
			drawing.setup(mainMC, this);

			setBackImage("web");
			changeVisibility(false);

			drawControl = new DrawControl();
			drawControl.setup(mainMC, this);
			this.addChild(drawControl);


			imageControl = new ImageControl();

			imageControl.setup(mainMC, this);
			this.addChild(imageControl);


			Drawing_Window_UI_MC = new Drawing_Window_UI();
			this.addChild(Drawing_Window_UI_MC);
			Drawing_Window_UI_MC.setup(mainMC, this);

			backgroundImageUI = new BackGroundImageUI(mainMC, this);
			this.addChild(backgroundImageUI);


			Drawing_Window_MouseMode_UI_MC = new Drawing_Window_MouseMode_UI();

			this.addChild(Drawing_Window_MouseMode_UI_MC);
			Drawing_Window_MouseMode_UI_MC.setup(mainMC, this);




		}

		public function setImage(url: String) {
			trace("url = " + url);
			//backImage.loadUrl(url);
		}

	}

}