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
	import flash.filesystem.*;

	public class Drawing_Window_UI extends MovieClip {
		var mainMC: MovieClip;
		var myParent: MovieClip;

		var MouseOffset: Point = new Point(0, 0);
		var DownFlag: Boolean = false;
		var UIoriginY;
		var UIoriginX;
		var UIoutX;

		var iPadFileName: String = "pic_";
		//var iPadFileCntr:Number = 0;
		var _S3PostRequest: S3PostRequest;

		public function Drawing_Window_UI() {

		}

		public function setup(MainMC: MovieClip, aParent: MovieClip) {
			mainMC = MainMC;
			myParent = aParent;
			this.clearButton.button.stop();
			this.saveButton.button.stop();
			this.loadButton.button.stop();
			this.playDrawingButton.button.stop();
			this.imageButton.button.stop();
			this.drawingUIbutton.button.stop();
			this.deleteButton.button.stop();
			this.nextButton.button.stop();
			this.uploadButton.button.stop();
			// constructor code
			this.clearButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.clearButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.saveButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.saveButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.loadButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.loadButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.playDrawingButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.playDrawingButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.imageButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.imageButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.deleteButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.deleteButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.nextButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.nextButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);

			this.uploadButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.uploadButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);

			this.drawingUIbutton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.drawingUIbutton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);

			UIoriginY = 30;
			UIoriginX = 1024;
			UIoutX = UIoriginX - this.width - 20;


			moveIn();

			this.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownUI);
			this.addEventListener(MouseEvent.MOUSE_UP, mouseUpUI);
			this.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveUI);
		}

		public function moveIn() {
			this.x = UIoriginX;
			this.y = UIoriginY;
			myParent.drawControl.visible = false
		}
		public function moveOut() {
			this.x = UIoutX;
			this.y = UIoriginY;
			myParent.drawControl.visible = true
		}

		//////////

		public function sortEvent(event: MouseEvent): void {
			var n: String = event.target.parent.name;
			var o: String = String(event.type);
			n = n + "_" + o;

			switch (n) {
				case "clearButton_mouseDown":
					this.clearButton.button.gotoAndStop(2);
					myParent.drawing.clearArray();

					break;
				case "clearButton_mouseUp":
					this.clearButton.button.gotoAndStop(1);

					break;
				case "saveButton_mouseDown":
					//var tempNam:String = iPadFileName + iPadFileCntr;
					myParent.fileIO.saveFile(myParent.drawing.getDrawing(), iPadFileName);

					this.saveButton.button.gotoAndStop(2);

					break;
				case "saveButton_mouseUp":
					this.saveButton.button.gotoAndStop(1);

					break;

				case "uploadButton_mouseDown":
					var nam: String = myParent.fileIO.currentFileName;
					trace("name" + nam);
					var file: File;
					file = File.documentsDirectory.resolvePath(iPadFileName + "3.txt");
					_S3PostRequest = new S3PostRequest("AKIAJLRHDJBD4EYFZ7WQ", "drawingmachine", file.name);
					_S3PostRequest.upload(file);
				
			/*
var _loadImage:loadImage = new loadImage();
				for (var a:int; a<20; a++){ 
					var s:String = "tintin"+ a+ ".jpg";
					//trace(s);
					_loadImage.fileExistsServer(s);
					
				}
				*/

					//myParent.backImage.loadUrl("https:s3-us-west-2.amazonaws.com/drawingmachine/tintin2.jpg");
					//https://s3-us-west-2.amazonaws.com/drawingmachine/tintin.txt
					break;

				case "uploadButton_mouseDown":

					break;

				case "playDrawingButton_mouseDown":
					myParent.drawing.setTheMode("playDrawing");
					this.playDrawingButton.button.gotoAndStop(2);

					break;
				case "playDrawingButton_mouseUp":
					this.playDrawingButton.button.gotoAndStop(1);

					break;
				case "loadButton_mouseDown":
					myParent.drawing.loadDrawing(myParent.fileIO.readCurrent(iPadFileName)); //readRandom(iPadFileName ));
					this.loadButton.button.gotoAndStop(2);

					break;
				case "loadButton_mouseUp":
					this.loadButton.button.gotoAndStop(1);

					break;
				case "imageButton_mouseDown":
					myParent.drawing.setTheMode("record");
					myParent.drawing.loadDrawing(myParent.fileIO.readLibNext());
					this.imageButton.button.gotoAndStop(2);

					break;
				case "imageButton_mouseUp":
					this.imageButton.button.gotoAndStop(1);

					break;

				case "drawingUIbutton_mouseUp":
					this.drawingUIbutton.button.gotoAndStop(1);

					if (this.x < UIoriginX - 30) {
						moveIn();
					} else {
						moveOut();
					}

					break;

				case "drawingUIbutton_mouseDown":
					this.drawingUIbutton.button.gotoAndStop(2);

					break;

				case "deleteButton_mouseDown":

					myParent.fileIO.deleteCurrentFile(iPadFileName)
					this.deleteButton.button.gotoAndStop(2);

					break;
				case "deleteButton_mouseUp":
					this.deleteButton.button.gotoAndStop(1);

					break;

				case "nextButton_mouseDown":
					myParent.drawing.loadDrawing(myParent.fileIO.readNext(iPadFileName));

					this.nextButton.button.gotoAndStop(2);

					break;
				case "nextButton_mouseUp":
					this.nextButton.button.gotoAndStop(1);

					break;

			}
		}

		////////////
		public function mouseDownUI(event: MouseEvent): void {
			MouseOffset.x = stage.mouseX - this.x;
			MouseOffset.y = stage.mouseY - this.y;
			DownFlag = true;
		}
		public function mouseUpUI(event: MouseEvent): void {
			DownFlag = false;

		}

		public function mouseMoveUI(event: MouseEvent): void {
			if (DownFlag == true) {

				this.x = stage.mouseX - MouseOffset.x;
				this.y = stage.mouseY - MouseOffset.y;

			}
		}





	}

}