package {

	public class ImageData {
		var drawArray: Array = new Array();
		var scale:Number =1.0;
		public function ImageData() {
			// constructor code
		}
		
		public function scaleData(num:Number){
			scale = num;
		
	}
	
	
		public function addData(u: Array) {
			u[0] = u[0] / scale;
			u[1] = u[1] / scale;
			drawArray.push(u);
		}

		public function clearArray() {
			drawArray = [];
		}

		public function loadArray(newData: Array) {
			drawArray = newData;
		}


	//	public function getImageData(): Array {
	//		return (drawArray);
	//	}
		public function getLength(): Number {
			return (drawArray.length);
		}
		
		public function getX(playPos:Number):Number{
			return (drawArray[playPos][0] * scale);
		}
		public function getY(playPos:Number):Number{
			return (drawArray[playPos][1] * scale);
		}
		public function getZ(playPos:Number):Number{
			return (drawArray[playPos][2] );
		}
	}

}