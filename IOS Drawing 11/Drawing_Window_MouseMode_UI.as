package  {
		import flash.utils.*;
	import flash.display.*;
	import flash.net.*;
	import flash.media.*;
	import flash.system.*;
	import flash.events.*;
	import flash.filters.*;
	import flash.geom.Point;
	public class Drawing_Window_MouseMode_UI extends MovieClip{
		var mainMC: MovieClip;
		var myParent: MovieClip;
		var penMode:String = "finger";
		var pendownFlag:Boolean = false;
		var outX = 164;
		var inX = 40;
		var posY = 105
		public function Drawing_Window_MouseMode_UI() {
		}
		public function setup(MainMC, aParent) {
			mainMC = MainMC;
			myParent = aParent;
			// constructor code
			this.gotoAndStop(1);
			this.penDownButton.gotoAndStop(1);
			this.penDownButton.button.gotoAndStop(1);
		this.modeButton.button.gotoAndStop(1);
				this.modeButton.gotoAndStop(1);
			
			this.x = inX;
			this.y = posY;
			
			this.modeButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.modeButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.penDownButton.button.addEventListener(MouseEvent.MOUSE_UP, sortEvent);
			this.penDownButton.button.addEventListener(MouseEvent.MOUSE_DOWN, sortEvent);
			this.penDownButton.visible = false;
			
		}
		
		
		
		public function sortEvent(event: MouseEvent): void {
			var n: String = event.target.parent.name;
			var o: String = String(event.type);
			n = n + "_" + o;

			switch (n) {
				case "modeButton_mouseDown":
					this.modeButton.button.gotoAndStop(2);
				if (penMode == "arrow"){
					penMode = "finger"
					myParent.drawing.arrowOff();
					this.modeButton.gotoAndStop(1);
					
				}else{
					
					penMode = "arrow"
					myParent.drawing.arrowOn();
					this.modeButton.gotoAndStop(2);
				
					}
					break;
				case "modeButton_mouseUp":
					if (penMode == "arrow"){
						this.gotoAndStop(2);
							this.penDownButton.visible = true;
						this.x = outX;
					}else{
						this.x = inX;
						this.gotoAndStop(1);
						this.penDownButton.visible = false;
					}
					this.modeButton.button.gotoAndStop(1);
					break;
				case "penDownButton_mouseDown":
					if (pendownFlag == false){
						pendownFlag = true;
						myParent.drawing.setPenState(true);
						this.penDownButton.button.gotoAndStop(2);
						this.penDownButton.gotoAndStop(1);
					}else{
						pendownFlag = false;
						myParent.drawing.setPenState(false);
						this.penDownButton.button.gotoAndStop(1);
						this.penDownButton.gotoAndStop(1);
					}
					
					break;
				case "penDownButton_mouseUp":
					//this.penDownButton.button.gotoAndStop(1);
					
				
				this.penDownButton.gotoAndStop(1);
					break;
				
			}
		}

	}
	
}
