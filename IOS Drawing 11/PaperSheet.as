package  {
		import flash.utils.*;
	import flash.display.*;
	import flash.media.*;
	import flash.system.*;
	import flash.events.*;
	import flash.filters.*;
	import flash.geom.Point;
	public class PaperSheet extends Sprite{
var mainMC: MovieClip;
		var myParent: Sprite;
		
		var paperLoc:Point = new Point(100, 510);
		
		
		public function PaperSheet(MainMC, aParent) {
			mainMC = MainMC;
			myParent = aParent;
		}
		
		
		public function drawLines(imageData:ImageData) {

			this.graphics.clear();
			this.graphics.lineStyle(1, 0xFF0000, 1);
			var l = imageData.getLength();

			if (l > 0) {

			
				this.graphics.moveTo(paperLoc.x +imageData.getX(0), paperLoc.y -imageData.getY(0));
				
				for (var c = 1; c < l; c++) {
					var lastZstate = imageData.getZ(c - 1);

					

					if (imageData.getZ(c) < 1) {
						
						if (lastZstate > 0) {
							this.graphics.moveTo(paperLoc.x +imageData.getX(c), paperLoc.y - imageData.getY(c));
						} else {
							this.graphics.lineTo(paperLoc.x + imageData.getX(c), paperLoc.y - imageData.getY(c));
						}
					} else {

						this.graphics.moveTo(paperLoc.x + imageData.getX(c), paperLoc.y - imageData.getY(c));
					}
				}

			}
		}
		

	}
	
}
