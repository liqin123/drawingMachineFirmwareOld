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


	public class CameraClass extends Sprite {
		var camera: Camera;
		var video = new Video();
		var bitmapData:BitmapData = new BitmapData(video.width,video.height);
		var bitmap:Bitmap;
		var paused: Boolean = false;//
		public function CameraClass() {
			// constructor code
			camera = Camera.getCamera();
			video.attachCamera(camera);
			this.addChild(video); // 'this' would be a Sprite or UIComponent, etc...
			bitmapData.draw(video);
			bitmap = new Bitmap(bitmapData);
			this.addChild(bitmap);
			//bitmap.x = 200;
			bitmap.visible = false;
			paused = false;//
		}


public function togglePauseCam():Bitmap{
	
	if (paused == false) {
		paused = true;
		bitmapData.draw(video);
		//bitmap = new Bitmap(bitmapData);
		bitmap.bitmapData = bitmapData;
		bitmap.visible = true;
	video.visible = false;
	}else{
		paused = false;
		bitmap.visible = false;
	video.visible = true;
	}
trace("paused = "+ paused);
	
return(bitmap);
			

		}

	}

}