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


	import flash.desktop.Clipboard;
	import flash.desktop.ClipboardFormats;





	public class BackGroundImageUI extends MovieClip {
		var mainMC: MovieClip;
		var myParent: MovieClip;
		var DownFlag: Boolean = false;
		var MouseOffset: Point = new Point();

		var UIoriginY;
		var UIoriginX;
		var UIoutX;


		public function BackGroundImageUI(MainMC: MovieClip, aParent: MovieClip) {
			mainMC = MainMC;
			myParent = aParent;



			UIoriginY = 120;
			UIoriginX = 1024;
			UIoutX = UIoriginX - this.width - 20;

			moveIn();


			this.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownUI);
			this.addEventListener(MouseEvent.MOUSE_UP, mouseUpUI);
			this.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveUI);

			// constructor code

			this.pasteButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.pasteButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.pasteButton.button.gotoAndStop(1);
			this.pasteButton.visible = true;
			
			this.seeButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.seeButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
				this.seeButton.button.gotoAndStop(1);
				
			this.sourceButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.sourceButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.sourceButton.button.gotoAndStop(1);
			this.sourceButton.gotoAndStop(2);

			this.url.text = "http://marciatest1.weebly.com/uploads/1/4/7/3/14733758/4655472.jpg";

			this.imageUIbutton.button.gotoAndStop(1);
			this.imageUIbutton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.imageUIbutton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);

this.pauseLiveButton..gotoAndStop(1);
this.pauseLiveButton.button.gotoAndStop(1);
			this.pauseLiveButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.pauseLiveButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
this.pauseLiveButton.visible = false;

			this.seeButton.gotoAndStop(1);
		}

		public function moveIn() {
			this.x = UIoriginX;
			this.y = UIoriginY;
			myParent.imageControl.visible = false
		}
		public function moveOut() {
			this.x = UIoutX;
			this.y = UIoriginY;
			myParent.imageControl.visible = true
		}

		public function sortEvent(event: MouseEvent): void {
			var n: String = event.target.parent.name;
			var o: String = String(event.type);
			n = n + "_" + o;

			switch (n) {
				case "pasteButton_mouseDown":

					if (Clipboard.generalClipboard.hasFormat(ClipboardFormats.TEXT_FORMAT)) {
						var a: String = String((Clipboard.generalClipboard.getData(ClipboardFormats.TEXT_FORMAT)));
						this.url.text = a;
						myParent.setImage(a);
						myParent.changeVisibility(true); // backImage.visible = true;
					}

					this.pasteButton.button.gotoAndStop(2);
					//myParent.drawing.clearArray();
					break;

				case "pasteButton_mouseUp":
					
					this.pasteButton.button.gotoAndStop(1);
					//myParent.drawing.clearArray();
					break;

				case "seeButton_mouseDown":

					if (myParent.currentBack_visibility == true) {
						myParent.changeVisibility(false);
						this.seeButton.gotoAndStop(1);
					} else {
						myParent.changeVisibility(true)
						this.seeButton.gotoAndStop(2);
					}


					this.seeButton.button.gotoAndStop(2);
					//myParent.drawing.clearArray();
					break;

				case "seeButton_mouseUp":
					
				
					this.seeButton.button.gotoAndStop(1);
					//myParent.drawing.clearArray();
					break;
				case "sourceButton_mouseDown":
					//myParent.backImage.visible = false;
					this.sourceButton.button.gotoAndStop(2);
					//myParent.drawing.clearArray();
					break;

				case "sourceButton_mouseUp":
					this.sourceButton.button.gotoAndStop(1);
				myParent.swapBackImage();
				
				if (myParent.getCurrentBackName() == "camera"){
					this.sourceButton.gotoAndStop(1);
					this.pauseLiveButton.visible = true;
					this.pasteButton.visible = false;
				}else{
					this.sourceButton.gotoAndStop(2);
					this.pauseLiveButton.visible = false;
					this.pasteButton.visible = true;
				}
				this.seeButton.gotoAndStop(2);
				
					//myParent.drawing.clearArray();
					break;

				case "imageUIbutton_mouseUp":
					this.imageUIbutton.button.gotoAndStop(1);

					if (this.x < UIoriginX - 30) {
						moveIn();
						//myParent.Drawing_Window_UI_MC.moveOut();
					} else {
						moveOut();
						//myParent.Drawing_Window_UI_MC.moveIn();
					}

					break;

				case "imageUIbutton_mouseDown":
					this.imageUIbutton.button.gotoAndStop(2);

					break;
				case "pauseLiveButton_mouseDown":
					
				if (myParent._camera.paused == false){
					this.pauseLiveButton.gotoAndStop(2);
				}else{
					this.pauseLiveButton.gotoAndStop(1);
				}
			
				
				
					this.pauseLiveButton.button.gotoAndStop(2);
					break;
				case "pauseLiveButton_mouseUp":
					myParent._camera.togglePauseCam();//addChild(bitmap1);
					this.pauseLiveButton.button.gotoAndStop(1);
				
					break;
			}
		}




		public function mouseDownUI(event: MouseEvent): void {
			trace(mouseX, this.x)
			MouseOffset.x = stage.mouseX - this.x;
			MouseOffset.y = stage.mouseY - this.y;
			DownFlag = true;
		}
		public function mouseUpUI(event: MouseEvent): void {
			DownFlag = false;

		}

		public function mouseMoveUI(event: MouseEvent): void {
			if (DownFlag == true) {
				//trace(Drawing_Window_UI_MC.x +"  " +  mouseX +"  " +  MouseOffset.x);
				this.x = stage.mouseX - MouseOffset.x;
				this.y = stage.mouseY - MouseOffset.y;

			}
		}
	}

}