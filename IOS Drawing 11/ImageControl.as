package {
	import flash.xml.*;
	import flash.utils.*;
	import flash.display.*;
	import flash.net.*;
	import flash.media.*;
	import flash.system.*;
	import flash.events.*;
	import flash.filters.*;
	import flash.geom.Point;
	import flash.events.*;
	public class ImageControl extends MovieClip {
		var mainMC: MovieClip;
		var myParent: MovieClip;

		var drawControlLocOrigin: Point = new Point(850, 510);
		var drawControlLoc: Point = new Point(850, 510);
		var drawControlStartDrag = new Point(850, 510);

		var moveControlMoveFlag: Boolean = false;
		var scaleFlag: Boolean = false;

		public function ImageControl() {
			// constructor code
		}
		public function setup(MainMC: MovieClip, aParent: MovieClip) {
			mainMC = MainMC;
			myParent = aParent;

			this.moveButton.gotoAndStop(1);
			this.scaleButton.gotoAndStop(1);
			this.x = drawControlLoc.x;
			this.y = drawControlLoc.y;
			this.moveButton.addEventListener(MouseEvent.MOUSE_DOWN, moveControlDown);
			this.moveButton.addEventListener(MouseEvent.MOUSE_UP, moveControlUp);
			this.moveButton.addEventListener(MouseEvent.MOUSE_MOVE, moveControlMove);

			this.scaleButton.addEventListener(MouseEvent.MOUSE_DOWN, scaleControlDown);
			this.scaleButton.addEventListener(MouseEvent.MOUSE_UP, scaleControlUp);
			this.scaleButton.addEventListener(MouseEvent.MOUSE_MOVE, scaleControlMove);
		}



		public function moveControlDown(event: MouseEvent): void {
			drawControlStartDrag.x = drawControlLocOrigin.x;
			drawControlStartDrag.y = drawControlLocOrigin.y;
			moveControlMoveFlag = true;
		}
		public function moveControlUp(event: MouseEvent): void {
			moveControlMoveFlag = false;
			this.x = drawControlLocOrigin.x;
			this.y = drawControlLocOrigin.y;
		}
		public function moveControlMove(event: MouseEvent): void {

			if (moveControlMoveFlag == true) {

				drawControlLoc.x = stage.mouseX;
				drawControlLoc.y = stage.mouseY;
				this.x = drawControlLoc.x;
				this.y = drawControlLoc.y;

				myParent.moveBack(drawControlLoc.x - drawControlStartDrag.x, drawControlLoc.y - drawControlStartDrag.y);
				//myParent.backImage.x += drawControlLoc.x - drawControlStartDrag.x;
				//myParent.backImage.y += drawControlLoc.y - drawControlStartDrag.y;
				drawControlStartDrag.x = drawControlLoc.x;
				drawControlStartDrag.y = drawControlLoc.y;
			}
		}



		public function scaleControlDown(event: MouseEvent): void {
			scaleFlag = true;
		}


		public function scaleControlUp(event: MouseEvent): void {
			this.scaleButton.x = this.moveButton.x;
			scaleFlag = false;
		}

		public function scaleControlMove(event: MouseEvent): void {
			if (scaleFlag == true) {

				var temp: Number = (mouseX / 50.0); // not stage
				myParent.scaleImage(Math.pow(2, temp));


				this.scaleButton.x = this.mouseX;
			}
		}



	}

}