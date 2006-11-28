/* ************************************************************************ */
/*																			*/
/*  Neko-Aspell - a Neko VM library that wraps GNU Aspell					*/
/*  Copyright (c)2006,2007													*/
/*  Edwin van Rijkom														*/
/*																			*/
/*  The development of this library was kindly sponsored by Artifex			*/
/*  (www.afxfirm.com)														*/
/*																			*/
/* This library is free software; you can redistribute it and/or			*/
/* modify it under the terms of the GNU Lesser General Public				*/
/* License as published by the Free Software Foundation; either				*/
/* version 2.1 of the License, or (at your option) any later version.		*/
/*																			*/
/* This library is distributed in the hope that it will be useful,			*/
/* but WITHOUT ANY WARRANTY; without even the implied warranty of			*/
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU		*/
/* Lesser General Public License or the LICENSE file for more details.		*/
/*																			*/
/* ************************************************************************ */

package aspell;

class Speller {
	var s: Void;
	
	public function new(s) {
		this.s = s;		
	}
	
	static var _nas_speller_check = neko.Lib.load("aspell","nas_speller_check",2);
	public function check(word: String): Int {		
		return _nas_speller_check(s,untyped word.__s);
	}
	
	static var _nas_speller_suggest = neko.Lib.load("aspell","nas_speller_suggest",2);
	public function suggest(word: String): Array<String> {		
		var result =  _nas_speller_suggest(s,untyped word.__s);
		var a : Array<String> = untyped Array.new1(result,__dollar__asize(result));
			for( i in 0...a.length ) a[i] = new String(a[i]);
			return a;
	}
}