package {
	import flash.utils.*;
	import flash.display.*;
	import flash.media.*;
	import flash.system.*;
	import flash.events.*;
	import flash.filters.*;
	import flash.geom.Point;
	public class Arduino_Arm extends MovieClip {
	
		var mainMC:MovieClip;
		var myParent: Drawing;
		var shoulderServoAngle:Number;
		var elbowServoAngle:Number;
		
		public function Arduino_Arm() {
			// constructor code
		}
		public function setup(MainMC, aParent) {
		mainMC = MainMC;
			myParent = aParent;
			
			//trace();
			}

	public function setArms(ax:Number,ay:Number,az:Number){
		
		computeArmAngles(ax,ay);
		bone1.rotation = 360 -(shoulderServoAngle);//((shoulderServoAngle * (Math.PI * 2))/180.0);
		bone2.rotation = elbowServoAngle -90;//((elbowServoAngle * (Math.PI * 2))/180.0);
		bone2.x = Math.cos(toRadian(bone1.rotation)) * myParent.armLength;
		bone2.y = Math.sin(toRadian(bone1.rotation)) * myParent.armLength;
		
	}
	
	public function toRadian(num:Number):Number{
		var ret:Number;
		ret =  (num * (Math.PI * 2))/360;
		return ret;
		
	}

		public function computeArmAngles(ax:Number,ay:Number) {
			
			var a1: Number ;
			 var a2:Number;

			if (ax == 0) {
				a1 = Math.PI / 2;
			} else {
				a1 = Math.atan2(ay, ax);
			}
			a2 = Math.acos(Math.sqrt((ax * ax) + (ay * ay)) / (2 * myParent.armLength));

			shoulderServoAngle = (a1 + a2) * 180 / Math.PI;
			elbowServoAngle = (Math.PI / 2 + a2 - a1) * 180 / Math.PI;
			//trace("l",shoulderServoAngle,elbowServoAngle);
			if (elbowServoAngle < 0) {
				elbowServoAngle += 180;
			};
			

		}

	}

}