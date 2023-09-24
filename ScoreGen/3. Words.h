#pragma region head

	#pragma once

	using namespace System;

	#include "1. Tools.h"

#pragma endregion

namespace ScoreGenLib {
namespace Music {
namespace Words {

	// Indications textuelles (classes de base à dériver)
	public ref class Words abstract {

		// Champs
		protected:
			String^ _text;

		// Constructeurs
		protected:
			Words(String^ text) :
				_text(text)
			{ }

		// Accesseur
		public:
			property String^ text {
				String^ get() {
					return _text;
				}
			}

		// Conversion implicite en tableau singleton (--> pouvoir toujours donner un texte unique là où plusieurs sont attendus)
		public:
			operator array<Words^, 1>^ () {
			
				return gcnew array<Words^, 1>{ this };

			}

		// A implémenter
		public:
			virtual String^ convertToMusicXML() = 0;
			virtual bool isInNote                   () = 0;

	};
	public ref class Direction abstract : public Words {

		// Types
		protected:
			ref class Placement {
	
				// Variables
				private:
					int val;
	
				// Constructeur
				private:
					Placement(int val) :
						val(val)
					{ }
	
				// Valeurs
				public:
					static Placement^ normal = gcnew Placement(1);
					static Placement^ below  = gcnew Placement(2);
	
				// Opérateurs
				public:
					static inline bool operator ==  (Placement^ a, Placement^ b) {
	
						return a->val == b->val;
	
					}
					static inline bool operator !=  (Placement^ a, Placement^ b) {
				
						return a->val != b->val;
	
					}
					static inline      operator int (Placement^ a) {
	
						return a->val;
	
					}

				// Conversion en texte MusicXML
				public:
					String^ convertToMusicXML() {
			
						if      (this == normal) return "";
						else if (this == below ) return " placement=\"below\"";
						else                     throw gcnew Exception;

					}
	
			};

		// Constructeurs
		protected:
			Direction(String^ text) :
				Words(text)
			{ }

		// Implémentations
		public:
			virtual String^ convertToMusicXML() override {
			
				return
					"<direction" + getPlacement()->convertToMusicXML() + ">" +
						"<direction-type>" +
							convertToMusicXML_XMLcontent_inline() +
						"</direction-type>" +
					"</direction>";

			}
			virtual bool isInNote() override {
				
				return false;

			}

		// A implémenter
		protected:
			virtual Placement^ getPlacement() = 0;
			virtual String^ convertToMusicXML_XMLcontent_inline() = 0;

	};
		
	// Indications textuelles de type <direction>
	public ref class Normal : public Direction {

		// Constructeur
		public:
			Normal(String^ text) :
				Direction(text)
			{ }

		// Implémentations : conversion en texte MusicXML
		protected:
			virtual Placement^ getPlacement() override {
				
				return Placement::normal;

			}
			virtual String^ convertToMusicXML_XMLcontent_inline() override {
			
				return
					"<words>" +
						text +
					"</words>";
		
			}

	};
	public ref class Dynamic : public Direction {
	
		// Champs
		private:
			bool _isStandardDynamic;
		private:
			static array<String^, 1>^ standardDynamics = gcnew array<String^, 1> {
				"pppppp",
				"ppppp"	,
				"pppp"	,
				"ppp"	,
				"pp"	,
				"p"		,
				"mp"	,
				"mf"	,
				"f"     ,
				"ff"	,
				"fff"	,
				"ffff"	,
				"fffff"	,
				"ffffff",
				"fp"	,
				"fz"	,
				"rf"	,
				"rfz"	,
				"sf"	,
				"sffz"	,
				"sfp"	,
				"sfpp"	,
				"sfz"
			};

		// Constructeur
		public:
			Dynamic(String^ text) :
				Direction(text->Trim())
			{
			
				_isStandardDynamic = false;
				for each (String^ standardDynamic in standardDynamics) {
					if (_text == standardDynamic) {
						_isStandardDynamic = true;
						break;
					}
				}
							
			}

		// Accesseurs
		public:
			property bool isStandardDynamic {
				bool get() {
					return _isStandardDynamic;
				}
			}

		// Implémentations : conversion en texte MusicXML
		protected:
			virtual Placement^ getPlacement() override {
				
				if (_isStandardDynamic) return Placement::normal;
				else                    return Placement::below ;

			}
			virtual String^ convertToMusicXML_XMLcontent_inline() override {

				if (_isStandardDynamic) {
					return
						"<dynamics>" +
							"<" + _text + "/>" +
						"</dynamics>";
				} else {
					return
						"<words font-family=\"Opus Text Std\">" +
							_text +
						"</words>";
				}
		
			}

	};
	public ref class Italic : public Direction {

		// Constructeur
		public:
			Italic(String^ text) :
				Direction(text)
			{ }

		// Implémentations : conversion en texte MusicXML
		protected:
			virtual Placement^ getPlacement() override {
				
				return Placement::below;

			}
			virtual String^ convertToMusicXML_XMLcontent_inline() override {
			
				return
					"<words font-style=\"italic\">" +
						text +
					"</words>";
		
			}

	};
	public ref class Tempo : public Direction {
		
		// Champs
		private:
			String^ _beatUnit;
			double  _perMinute;

		// Constructeur
		public:
			Tempo(String^ beatUnit, double perMinute) :
				Direction("")
			{

				initialize(beatUnit, perMinute);
			
			}
			Tempo(String^ text) :
				Direction("")
			{
			
				array<String^, 1>^ twoElements = text->Split(gcnew array<String^, 1>{"="}, StringSplitOptions::None);
				if (twoElements->Length != 2) throw gcnew Exception("A tempo should have the form 'beatUnit=perMinute', for example 'quarter=60' or 'w=28.5'.");
				
				String^ beatUnit = twoElements[0];
				double perMinute;
				if (!Double::TryParse(twoElements[1], perMinute) && !Double::TryParse(twoElements[1]->Replace(",", "."), perMinute) && !Double::TryParse(twoElements[1]->Replace(".", ","), perMinute)) {
					throw gcnew Exception("The second part of a tempo should be a number.");
				}

				initialize(beatUnit, perMinute);

			}
		private:
			void initialize(String^ beatUnit, double perMinute) {
				
				_beatUnit = beatUnit->TrimEnd()->ToLower();
				_perMinute = perMinute;

				check_beatUnit_throwIfWrong ();
				check_perMinute_throwIfWrong();

				generateBaseText();
			
			}

		// Accesseurs
		public:
			property String^ beatUnit {
				String^ get() {
					return _beatUnit;
				}
			}
			property double perMinute {
				double get() {
					return _perMinute;
				}
			}

		// Fonctions privées
		private:
			void check_beatUnit_throwIfWrong() {

				// Vérifier et/ou modifier la valeur rythmique
				if      (_beatUnit == "long"    || _beatUnit == "l"  ) throw gcnew Exception("The rhythmic part of a tempo indication shouldn't be the largest rhythmic value 'long'.");
				else if (_beatUnit == "breve"   || _beatUnit == "b"  ) _beatUnit = "breve";
				else if (_beatUnit == "whole"   || _beatUnit == "w"  ) _beatUnit = "whole"  ;
				else if (_beatUnit == "half"    || _beatUnit == "h"  ) _beatUnit = "half"   ;
				else if (_beatUnit == "quarter" || _beatUnit == "q"  ) _beatUnit = "quarter";
				else if (_beatUnit == "eighth"  || _beatUnit == "e"  ) _beatUnit = "eighth" ;
				else if (_beatUnit == "16th"                         ) /* ok */; 
				else if (_beatUnit == "32nd"                         ) /* ok */; 
				else if (_beatUnit == "64th"                         ) /* ok */; 
				else if (_beatUnit == "128th" || _beatUnit == "256th") throw gcnew Exception("The rhythmic part of a tempo indication shouldn't be a very small rhythmic value : '128th' or '256th'."); 
				else                                                   throw gcnew Exception("The rhythmic part of a tempo indication should be a rhythmic value : long, breve, whole, half, quarter, eighth, 16th, or 32nd.");
				
			}
			void check_perMinute_throwIfWrong() {
				
				if      (_perMinute <  0      ) throw gcnew Exception("The tempo indication shouldn't be negative.");
				else if (_perMinute == 0      ) throw gcnew Exception("The tempo indication shouldn't be zero.");
				else if (_perMinute <  0.00001) throw gcnew Exception("The tempo indication shouldn't be less than 0.00001.");
				else if (_perMinute >  100000 ) throw gcnew Exception("The tempo indication shouldn't be more than 100000.");

			}
			void generateBaseText() {
				
				_text = _beatUnit + "=" + _perMinute;

			}

		// Implémentations : conversion en texte MusicXML
		protected:
			virtual Placement^ getPlacement() override {
				
				return Placement::normal;

			}
			virtual String^ convertToMusicXML_XMLcontent_inline() override {
				
				String^ perMinute_asText = ("" + _perMinute)->Replace(",", ".");

				return
					"<metronome>" +
						"<beat-unit>" +
							_beatUnit +
						"</beat-unit>" +
						"<per-minute>" +
							perMinute_asText +
						"</per-minute>" +
					"</metronome>";
		
			}

	};
	public ref class Comment : public Direction {

		// Constructeur
		public:
			Comment(String^ text) :
				Direction(text)
			{ }

		// Implémentations : conversion en texte MusicXML
		protected:
			virtual Placement^ getPlacement() override {
				
				return Placement::normal;

			}
			virtual String^ convertToMusicXML_XMLcontent_inline() override {
			
				return
					"<words font-family=\"Verdana\" font-size=\"8.5\" font-weight=\"bold\" underline=\"1\">" +
						"#" + text +
					"</words>";
		
			}

	};
		
	// Autres indications textuelles
	public ref class Lyrics : public Words {
	
		// Champs
		private:
			String^ _syllabic;

		// Constructeurs
		public:
			Lyrics(String^ text) :
				Words("")
			{
				
				int a = text->StartsWith("-");
				int b = text->EndsWith  ("-");

				if (a && b) {
					_text = text->Substring(1, text->Length - 2);
					_syllabic = "middle";
				} else if (a) {
					_text = text->Substring(1);
					_syllabic = "end";
				} else if (b) {
					_text = text->Substring(0, text->Length - 1);
					_syllabic = "begin";
				} else {
					_text = text;
					_syllabic = "single";
				}
			
			}
			
		// Implémentations
		public:
			virtual bool isInNote() override {
				
				return true;

			}

		// Conversion en texte MusicXML
		public:
			virtual String^ convertToMusicXML() override {
			
				return
					"<lyric>" +
						"<syllabic>" +
							_syllabic +
						"</syllabic>" +
						"<text>" +
							_text +
						"</text>" +
					"</lyric>";

			}

	};
		
}
}
}
