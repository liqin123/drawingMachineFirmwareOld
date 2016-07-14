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

	public class saving {
		var mainMC: MovieClip;
		var file: File;
		var fileStream: FileStream;
		var str: String;
		var lib: MovieClip;
		var libNum = 1;
		var currentFileNum: Number = 0;
		var currentFileName:String = "";
		public function saving(MainMC) {
			mainMC = MainMC;
			fileStream = new FileStream();
			lib = new drawingData();
			//trace(lib.t1.text);
			// constructor code
		}


		function lastFileNum(aNam: String): Number {
			var ret = -1;
			for (var i: Number = 0; i < 50; i++) {
				var k: String = aNam + String(i) + ".txt";

				file = File.documentsDirectory.resolvePath(k);

				if (file.exists) {} else {
					ret = i;
					trace("there are " + (i - 1) + " pictures on iPad");
					i = 1000; // exit
				}
			}
			return (ret);
		}

		function checkFile(aNam: String): Boolean {
			var ret: Boolean = false;
			file = File.documentsDirectory.resolvePath(aNam + ".txt");
			if (file.exists) {
				ret = true;
			}
			return ret;
		}

		function saveFile(aList: Array, aNam: String): void {
			var temp: Number = lastFileNum(aNam);

			if (temp != -1) {
				currentFileNum = temp;
				currentFileName = aNam +  String(currentFileNum);
				file = File.documentsDirectory.resolvePath(currentFileName + ".txt");
				fileStream.open(file, FileMode.WRITE);

				for each(var item: String in aList) {
					//trace(item + "\n");
					fileStream.writeUTFBytes(item + "\n")
				}
				fileStream.close();
			}
		}

		////directory.deleteDirectory(true); 
		function deleteCurrentFile(aNam: String) {
			 
			if (checkFile(aNam + String(currentFileNum))) {
				
				currentFileName = aNam +  String(currentFileNum);
				file = File.documentsDirectory.resolvePath(currentFileName + ".txt");
				file.deleteFile(); 
			}
		}

		function readNext(aNam: String): Array {
			var temp: Number = lastFileNum(aNam);
			currentFileNum++;
			if (currentFileNum > temp) {
				currentFileNum = 0;
				
			}
			currentFileName = aNam +  String(currentFileNum);
			
			var ret = readFile(currentFileName);
			return (ret);
		}

		function readRandom(aNam: String): Array {
			var temp: Number = lastFileNum(aNam);
			currentFileNum = Math.floor(Math.random() * temp);
			currentFileName = aNam + String(currentFileNum);
			var ret = readFile(currentFileName);
			return (ret);
		}
 function readCurrent(aNam: String): Array {
	 currentFileName = aNam + String(currentFileNum);
	 var ret = readFile(currentFileName);
			return (ret);
 }

		function readFile(aNam: String): Array {
			file = File.documentsDirectory.resolvePath(aNam + ".txt");
			if (file.exists) {

				fileStream.open(file, FileMode.READ);
				str = fileStream.readMultiByte(file.size, File.systemCharset);
				//mainMC.debug.text = ("readFile");
				fileStream.close();

				mainMC.debug.text = (str);
			} else {
				str = ("300\n300\n1000");
			}
			//str = ("300\n300\n1000");

			return (stringToArry(str));
		}

		function readLibNext(): Array {
			if (libNum > 5) {
				libNum = 1;
			}

			var b = lib.getChildByName("t" + libNum);
			var c: String = (b.text);
			//var a:String = lib.t1.text;

			libNum++
			return (stringToArryLib(c));
		}
		function stringToArryLib(tempStr: String): Array {
			var params: Array = tempStr.split("\r");

			//trace("prams" + params);
			var drawArray: Array = new Array();
			for (var i: int = 0; i < (params.length) - 1; i = i + 3) {
				var n: Number = 0;
				var t: Number = Number(params[i + 2]);
				if (t < 1) {
					n = 1000;
				}

				var u = new Array(Number(params[i]) - 50, 500 - Number(params[i + 1]), n);
				//trace(u);
				drawArray.push(u);
			}
			//trace("reading" + drawArray.length);
			//trace(drawArray);
			return (drawArray);
		}



		function stringToArry(tempStr: String): Array {
			var params: Array = tempStr.split("\n");
			//trace("prams" + params);
			var drawArray: Array = new Array();
			for (var i: int = 0; i < params.length - 1; i++) {
				var tempArray: Array = new Array();
				tempArray = (params[i].split(","));
				var u = new Array(Number(tempArray[0]), Number(tempArray[1]), Number(tempArray[2]));

				drawArray.push(u);
			}
			//trace("reading" + drawArray.length);
			//trace(drawArray[drawArray.length-1]);
			return (drawArray);
		}

	}

}