package {
	import flash.events.*;
	import flash.net.*;
	public class Networking {

		//object properties/variables
		var socket = new XMLSocket(); // XML Socket for Serial
		var Connected: Boolean = false; // Connection flag
		var Host: String = "localhost"; // Locolhost IP
		var Port: Number = 5332; // Port number
		var Name: String = ""; // Name
		var buffer: Array = new Array();


		public function Networking(nme:String,host:String,port:Number) {
			// constructor code
			Name = nme;
			Port = port;
			if (host != null) Host = host;
			setup();
		}
		
		private function setup():void {
            socket.addEventListener(Event.CONNECT, OnConnect);
			socket.addEventListener(Event.CLOSE, OnClose);
			socket.addEventListener(DataEvent.DATA, OnReceiveData);
			connect();
		}
		
		public function available():Number {
			return buffer.length;
		}
		
		public function read():String {
			if (buffer.length > 0) {
				buffer.reverse();
				var msg:String=String(buffer.pop());
				buffer.reverse();
				
			}
			return msg;
		}            
		public function write(receiver:String,msg:String):void {
			sendData("write||" + receiver + "|" + msg);
		}
		public function clearBuffer():void {
			buffer=[];
		}
		
		
		// *********************************************************************************************************
		
		
		private function connect():void {
			if (! Connected) {
				if (Host.length == 0) {
					trace("Enter host name or IP address !");
				} else {
					if (isNaN(Port)) {
						trace("Enter port !");
					} else if (Port < 1024 || Port > 65535) {
						trace("Port must be from 1024 to 65535 !");
					} else {
						trace("Connecting to communication server at "+Host+":"+Port+" . . .");
						socket.connect(Host,Port);
					}
				}
			} else {
				trace("Already connected !");
			}
		}
		private function disconnect():void {
			if (Connected) {
				trace("Disconnecting.");
				socket.close();
			} else {
				trace("Not connected !");
			}
		}
		public function sendData(dataStr:String):void {
			
			socket.send(dataStr);
		}
		
		private function sendData2(dataStr:String):void {
			dataStr = "[" + dataStr + "]";
			if (Connected) {
				if (dataStr.length) {
					socket.send(dataStr);
				}
			} else {
				trace("Not connected!");
			}
		}
		private function OnReceiveData(e:DataEvent):void {
			var dataStr:String = e.data;
			var dataStrs = dataStr.split("|");
			if (dataStrs.length == 2) {
				if (dataStrs[0] == Name) {
					var n:String=String(dataStrs[1]);
					buffer.push(n);
					// stop buffer over-flow
					if (buffer.length > 512) {
						buffer.pop();
					}
				}
			}
		}
		private function OnConnect(e:Event) {
			Connected=true;
		}
		private function OnClose(e:Event) {
			Connected=false;
		}
	}

}