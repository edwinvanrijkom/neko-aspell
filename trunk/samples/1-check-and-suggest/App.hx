class App {
		
	static var words_en = 	
		[ "miscellanious"
		, "cucumber"
		, "aristocrat"
		, "lawn-mower"
		, "paralel"
		, "GNU"
		];
		
	static var lib_path = 
		{ Windows: "../../tools/libaspell-15.dll"
		, Mac: "../../tools/libaspell.15.dylib"
		};
	
	static var dict_dir = "../../tools/dictionary";		
			
	static function main() {
		checkWords(words_en,"en");		
	}
	
	static function checkWords(words: Array<String>,lid: String){
		trace("Checking word-list ...");
		trace("Language ID: "+lid+" ...\n");
		var path = Reflect.field(lib_path,neko.Sys.systemName());
		var config = new aspell.Config(path);
		/* 
		 * Configuration options are listed at: 
		 * http://aspell.net/man-html/The-Options.html#The-Options
		 *
		 */
		config.replace("lang",lid);
		config.replace("dict-dir",dict_dir);
		var error = config.createSpeller();
		if (error.number == 0) {
			trace("Speller created...");
			var speller = error.toSpeller();
			for (i in 0...words.length) {
				trace("Checking '"+words[i]+"' ...");
				if (speller.check(words[i])!=0)
					trace("PASS");
				else {
					trace("FAIL");
					var suggestions = speller.suggest(words[i]);
					trace("Speller suggests: ");
					for (j in 0...suggestions.length) {
						trace("    "+suggestions[j]);
					}
				}
			}
			trace("Done\n\n");
		} else {
			trace("Failed to construct speller: "+error.message);
		}
	}
}
