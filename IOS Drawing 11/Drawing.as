package {
	import flash.utils.*;
	import flash.display.*;
	import flash.media.*;
	import flash.system.*;
	import flash.events.*;
	import flash.filters.*;
	import flash.geom.Point;
	public class Drawing extends Sprite {
		var drawArea: Sprite;
		var DownFlag: Boolean = false;
		var mainMC: MovieClip;
		var myParent: MovieClip;
		var arm: Arduino_Arm;
		var armLength: Number = 250;
		var armLoc: Point = new Point(100, 510);
		//var paperLoc: Point = new Point(100, 510);
		var arm_mousePos: Point = new Point(300, 300);
		var paper_mousePos: Point = new Point(300, 300);
		var DownFlagServo: Boolean = false;
		var myMode: String = "";

		var mousePos: Point = new Point(0, 0);
		var lastPos: Point = new Point(0, 0);;
		var drawPaper: PaperSheet;
		var PenPosNum: Array = [1000, 0];
		var paperHandle: PaperLocation;
		var penDownState: Number = 0;
		var updateFlag: Boolean = false;
		var prop: Number;
		var playPos: Number = 0;

		var updateTimer: Timer;
		var updateRate: Number;

		var lastX: Number;
		var lastY: Number;
		var lastZ: Number;

		var sheetList: Array = new Array();
		var imageData: ImageData;
		var mouseTouch: Sprite;
		var myMousex: Number;
		var myMousey: Number;

		var penOffset: Point = new Point(0, 0);
		var _arrow: Arrow;
		var arrowTrackOffset: Point = new Point(300, 300);
		var lastMouse: Point = new Point(300, 300);

		var newMarkFlag: Boolean = false;
		
		var UpatedSinceUp:Boolean = false;
		public function Drawing() {
			trace();
		}

		public function setup(MainMC, aParent) {

			mainMC = MainMC;
			myParent = aParent;

			setTouchArea();



			drawArea = new Sprite();
			this.addChild(drawArea);
			setdrawArea();
			drawArea.alpha = 0.5;


			imageData = new ImageData();
			prop = 1000 / armLength;



			this.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownPen);
			this.addEventListener(MouseEvent.MOUSE_UP, mouseUpPen);
			this.addEventListener(MouseEvent.MOUSE_MOVE, mouseMovePen);
			//drawArea.addEventListener(MouseEvent.MOUSE_OUT, mouseOutPen);


			drawPaper = new PaperSheet(mainMC, this);
			this.addChild(drawPaper);

			//var temp:PaperSheet = new PaperSheet(mainMC,this);
			//sheetList.push(temp);			
			//this.addChild(temp);


			paperHandle = new PaperLocation;

			//this.setChildIndex(drawPaper,(this.numChildren-1));
			//lineStyle(thickness:Number = NaN, color:uint = 0, alpha:Number = 1.0, pixelHinting:Boolean = false, scaleMode:String = "normal", caps:String = null, joints:String = null, miterLimit:Number = 3)

			/*
			 */
			arm = new Arduino_Arm();
			this.addChild(arm);
			arm.setup(MainMC, this);
			this.addChild(paperHandle);
			paperHandle.x = drawPaper.paperLoc.x;
			paperHandle.y = drawPaper.paperLoc.y;

			arm.x = armLoc.x;
			arm.y = armLoc.y;
			arm.servo_MC.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownServo);
			arm.servo_MC.addEventListener(MouseEvent.MOUSE_UP, mouseUpServo);
			arm.servo_MC.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveServo);

			updateRate = 40;
			updateTimer = new Timer(updateRate);
			updateTimer.addEventListener(TimerEvent.TIMER, updateNow);
			updateTimer.start();

			setTheMode("record");



			_arrow = new Arrow();
			this.addChild(_arrow);
			_arrow.visible = false;
			_arrow.x = arrowTrackOffset.x;
			_arrow.y = arrowTrackOffset.y;
			_arrow.gotoAndStop(1);


		}
		public function updateNow(event: TimerEvent) {
			updateFlag = true;
			//trace(myMode);
			if (myMode == "playDrawing") {
				update();
			}
		}


		public function movePaper(offset: Point) {

			drawPaper.paperLoc.x += offset.x;
			drawPaper.paperLoc.y += offset.y;;

			paperHandle.x = drawPaper.paperLoc.x;
			paperHandle.y = drawPaper.paperLoc.y;
			drawPaper.drawLines(imageData); //????
		}

		public function updateCurrentPaper() {
			drawPaper.drawLines(imageData);
		}

		public function resetPaper() {
			drawPaper.paperLoc.x = armLoc.x;
			drawPaper.paperLoc.y = armLoc.y;
			paperHandle.x = drawPaper.paperLoc.x;
			paperHandle.y = drawPaper.paperLoc.y;
		}

		function setTheMode(aMode: String) {
			playPos = 0; // always restart drawing
			myMode = aMode;
		}




		function update() {
UpatedSinceUp = true;
			arm_mousePos.x = mousePos.x - armLoc.x;
			arm_mousePos.y = armLoc.y - mousePos.y;

			paper_mousePos.x = mousePos.x - drawPaper.paperLoc.x; // mouse from paper origin
			paper_mousePos.y = drawPaper.paperLoc.y - mousePos.y;

			if (getDist(new Point(mousePos.x, mousePos.y), armLoc) >= armLength * 2) {
				trace(">>> out of bounds");
				return;
			}




			switch (myMode) {
				case "moveArm":
					break;
				case "record":
					if (updateFlag == false) {
					
						break;
					}
					updateFlag = false;
					if (DownFlagServo) {
						return;
					}


					if (hasChanged() == true) {

						arm.setArms(arm_mousePos.x, arm_mousePos.y, PenPosNum[penDownState]);

						addToArray(paper_mousePos.x, paper_mousePos.y, PenPosNum[penDownState]);

						drawPaper.drawLines(imageData);
						moveDrawingMachie(arm_mousePos.x, arm_mousePos.y, PenPosNum[penDownState]);

					}
					//sheetList[0].drawLines(drawArray);
					break;

				case "playDrawing":
					if (playPos < imageData.getLength()) {

						var paperOffsetX: Number = (paperHandle.x - armLoc.x);
						var paperOffsetY: Number = (paperHandle.y - armLoc.y);



						if (imageData.getZ(playPos) != lastZ) {

							// special delay for change of z with no arm motion change
							if (imageData.getZ(playPos) > lastZ) {
								moveDrawingMachie(paperOffsetX + imageData.getX(playPos), imageData.getY(playPos) - paperOffsetY, lastZ); ///only fo the z
							} else {
								moveDrawingMachie(paperOffsetX + lastX, lastY - paperOffsetY, imageData.getZ(playPos)); ///only fo the z

							}

							updateTimer.delay = 1000;
							lastZ = imageData.getZ(playPos);

						} else {
							updateTimer.delay = updateRate;
							arm.setArms(paperOffsetX + imageData.getX(playPos), imageData.getY(playPos) - paperOffsetY, imageData.getZ(playPos));
							moveDrawingMachie(paperOffsetX + imageData.getX(playPos), imageData.getY(playPos) - paperOffsetY, imageData.getZ(playPos));
							lastX = imageData.getX(playPos);
							lastY = imageData.getY(playPos);
							lastZ = imageData.getZ(playPos);
							playPos++;
						}
					} else {

						updateTimer.delay = updateRate;
						moveDrawingMachie(lastX, lastY, 1000);
						playPos = 0;
						setTheMode("record")
					}

					break;
			}
		}


		public function moveDrawingMachie(a1: Number, a2: Number, a3: Number) {


			var s1 = int(a1 * prop);
			var s2 = int(a2 * prop);
			var s3 = int(a3);


			var send: String = (String(s1) + 'x' + String(s2) + 'y' + String(s3) + 'z' + ';');
			if (mainMC.netOnFlag == true) {
				mainMC.network.sendData(send);
			}
		}

		///// recording
		public function addToArray(mx, my, mz) {

			var u = new Array(mx, my, mz);

			imageData.addData(u);

		}

		public function clearArray() {
			resetPaper();
			imageData.clearArray();
			drawPaper.graphics.clear();
		}

		public function hasChanged(): Boolean {
			var ret: Boolean = false;
			
			if (newMarkFlag == true) {
				
				// start of a new mark does not need to change
				ret = true;
			} else {
				if ((mousePos.x != lastPos.x) || (mousePos.y != lastPos.y)) {
					ret = true;
					lastPos.x = mousePos.x;
					lastPos.y = mousePos.y;
				}
			}
			return (ret);
		}

		////
		public function setPenState(st: Boolean) {
			if (st == true) {
				penDownState = 1;
				_arrow.gotoAndStop(2);
				mousePos.x = _arrow.x; //
				mousePos.y = _arrow.y; //
				trace(mousePos);
				trace(_arrow.x, _arrow.y);
				trace();

			} else {
				penDownState = 0;
				_arrow.gotoAndStop(1);


			}
		}


		public function mouseDownPen(event: MouseEvent): void {
			DownFlag = true;
var mx = mouseX;
			var my = mouseY;
			//
			trace("down>>>>>>>>>>>>>" );
			
			if (UpatedSinceUp == false){
				// if it has not updated between last up and this then force an update
				update();
			}
			if (myParent.Drawing_Window_MouseMode_UI_MC.penMode == "finger") {

				mousePos.x = mx;
				mousePos.y = my + penOffset.y;
				penDownState = 1;

			} else {
				/// arrow mode
				// when the pen goesdown always reset the mouse origin
				lastMouse.x = mouseX;
				lastMouse.y = mouseY;
					mousePos.x = _arrow.x + arrowTrackOffset.x;
					mousePos.y = _arrow.y + arrowTrackOffset.y;

				
			}

			updateFlag = true; // force update to ignore the timer
			update();
			newMarkFlag = false;

		}


		public function mouseMovePen(event: MouseEvent): void {
			//if (DownFlag) {
			var mx = mouseX;
			var my = mouseY;

			if (myParent.Drawing_Window_MouseMode_UI_MC.penMode == "finger") {

				mousePos.x = mx;
				mousePos.y = my + penOffset.y;

			} else {
				if (DownFlag == true) {

					arrowTrackOffset.x = mx - lastMouse.x;
					arrowTrackOffset.y = my - lastMouse.y;

					mousePos.x = _arrow.x + arrowTrackOffset.x;
					mousePos.y = _arrow.y + arrowTrackOffset.y;

					_arrow.x = mousePos.x;
					_arrow.y = mousePos.y;

					lastMouse.x = mx;
					lastMouse.y = my;
				}

			}


			if (DownFlag == true) {
				update();
			} else {
				//update();
			}
			//}
		}

		public function mouseUpPen(event: MouseEvent): void {

			trace("UP<<<<<<<<<<<<<<<");
			if (myParent.Drawing_Window_MouseMode_UI_MC.penMode == "finger") {
				trace("+ in UP<<<<<<<<<<<<<<<");
				newMarkFlag = true;
				lastMouse.x = mouseX;
				lastMouse.y = mouseY;
				arrowTrackOffset.x = _arrow.x;
				arrowTrackOffset.y = _arrow.y;
				penDownState = 0;
				DownFlag = false;
				update();

			} else {

				DownFlag = false;
				///if (myParent.Drawing_Window_MouseMode_UI_MC.pendownFlag == false) {
				//	penDownState = 0;
				//}
				//lastMouse.x;
				//lastMouse.y = mouseY;
				//mousePos.x = mouseX - 1;
				//mousePos.y = mouseY + penOffset.y;


				//update();


			}
UpatedSinceUp = false;
		}




		public function arrowOff() {
			_arrow.visible = false;
		}
		public function arrowOn() {
			_arrow.visible = true;
		}


		public function mouseOutPen(event: MouseEvent): void {
			trace("OUT<<<<<<<<<<<<<<<");
			mousePos.x = mouseX - 1;
			mousePos.y = mouseY + penOffset.y;

			penDownState = 0;
			DownFlag = false;
			update();

		}




		////
		public function mouseDownServo(event: MouseEvent): void {

			DownFlagServo = true;
		}
		public function mouseUpServo(event: MouseEvent): void {
			DownFlagServo = false;
		}

		public function mouseMoveServo(event: MouseEvent): void {
			// this is just moving the screen servo around
			if (DownFlagServo == true) {
				armLoc.x = mouseX;
				armLoc.y = mouseY - 40;
				arm.x = armLoc.x;
				arm.y = armLoc.y;
				setdrawArea();
			}
		}

		function setdrawArea() {
			drawArea.graphics.clear();
			drawArea.graphics.beginFill(0xEEEEEE); //D9D9D9);///
			drawArea.graphics.drawCircle(armLoc.x, armLoc.y, armLength * 2);
			drawArea.graphics.endFill();
		}
		public function getDrawing(): Array {
			trace("?????????");
			return (imageData.drawArray); ////???????
		}

		public function loadDrawing(newData: Array): void {
			imageData.loadArray(newData);
			resetPaper();
			update();
			drawPaper.drawLines(imageData);
		}

		public function getDist(m: Point, p: Point): Number {
			var newX = (m.x - p.x);
			var newY = (m.y - p.y);
			return (Math.sqrt((newX * newX) + (newY * newY)));
		}

		function setTouchArea() {
			mouseTouch = new Sprite();
			this.addChild(mouseTouch);
			mouseTouch.graphics.clear();
			mouseTouch.graphics.beginFill(0xFAFAFA);
			mouseTouch.graphics.drawRect(0, 0, 1024, 768);
			mouseTouch.graphics.endFill();
			mouseTouch.alpha = 0;
		}


	}

}