package  {
		import flash.xml.*;
	import flash.utils.*;
	import flash.display.*;
	import flash.net.*;
	import flash.media.*;
	import flash.system.*;
	import flash.events.*;
	import flash.filters.*;
	import flash.geom.Point;
	public class Main extends MovieClip{
var network: Networking;
		
		var serverIP =  "esp8266.local";
		var port: Number = 1337;
		var drawWindow: Drawing_Window;
		var netOnFlag:Boolean = false; //false//true; //
		
		public function Main() {
			
			//Dev2Certificates.p12
			// Platform3
			// /Users/durrell/Documents/Work/DurrellWork/DrawingMachine/AppleCerts/durrell01dev.mobileprovision
			//com.luckybite.durrell01
			
			
			theNetwork();
			drawWindow = new Drawing_Window();
			this.addChild(drawWindow);
			drawWindow.setup(this);
			drawWindow.x = 0;
			drawWindow.y = 0;
			switch_Mode("drawing");
				
		}
		
		
		

		public function theNetwork(){
			if (netOnFlag == true){
			network = new Networking("ipad", serverIP, port);
			}
		}
		
		public function switch_Mode(a_mode){
			switch(a_mode){
				case "drawing":
					break;
			}
			
		}

	}
	
}
