#ifndef NEW_RHYTHMS_HPP
#define NEW_RHYTHMS_HPP

// include : NEW_tools

#include <string>

namespace NEW_ScoreGenLib {
namespace NEW_Music {

//	// Figure rythmique (exemples : noire, croche...)
//	class NEW_RhythmFigure {
//		// Champs
//		private:
//			int         _duration_1024th;
//			std::string _name;
//		// Constructeurs et conversions
//		public:
//			NEW_RhythmFigure(int duration_1024th) :
//				_duration_1024th(duration_1024th)
//			{
//				// Initialisation du nom, et vérifications
//				if      (_duration_1024th ==    4) _name = "256th"  ;
//				else if (_duration_1024th ==    8) _name = "128th"  ;
//				else if (_duration_1024th ==   16) _name = "64th"   ;
//				else if (_duration_1024th ==   32) _name = "32nd"   ;
//				else if (_duration_1024th ==   64) _name = "16th"   ;
//				else if (_duration_1024th ==  128) _name = "eighth" ;
//				else if (_duration_1024th ==  256) _name = "quarter";
//				else if (_duration_1024th ==  512) _name = "half"   ;
//				else if (_duration_1024th == 1024) _name = "whole"  ;
//				else if (_duration_1024th == 2048) _name = "breve"  ;
//				else if (_duration_1024th == 4096) _name = "long"   ;
//				else                                         throw gcnew Exception("To create a 'RhythmFigure', only the durations which are powers of 2 between 4 and 4096 are accepted.");
//			}
//		public:
//			operator Rhythm^();
//		// Accesseurs
//		public:
//			property int duration_1024th {
//				int get() {
//					return _duration_1024th;
//				}
//			}
//			property String^ name {
//				String^ get() {
//					return _name;
//				}
//			}
//		// Valeurs
//		public:
//			static RhythmFigure^ _256th   = gcnew RhythmFigure(   4);
//			static RhythmFigure^ _128th   = gcnew RhythmFigure(   8);
//			static RhythmFigure^ _64th    = gcnew RhythmFigure(  16);
//			static RhythmFigure^ _32nd    = gcnew RhythmFigure(  32);
//			static RhythmFigure^ _16th    = gcnew RhythmFigure(  64);
//			static RhythmFigure^ _eighth  = gcnew RhythmFigure( 128);
//			static RhythmFigure^ _quarter = gcnew RhythmFigure( 256);
//			static RhythmFigure^ _half    = gcnew RhythmFigure( 512);
//			static RhythmFigure^ _whole   = gcnew RhythmFigure(1024);
//			static RhythmFigure^ _breve   = gcnew RhythmFigure(2048);
//			static RhythmFigure^ _long    = gcnew RhythmFigure(4096);
//		// Opérateurs
//		public:
//			static inline bool operator == (RhythmFigure^ a, RhythmFigure^ b) {
//				return a->_duration_1024th == b->_duration_1024th;
//			}
//			static inline bool operator != (RhythmFigure^ a, RhythmFigure^ b) {
//				return a->_duration_1024th != b->_duration_1024th;
//			}
//		// Conversion en texte MusicXML (balise <type></type>)
//		public:
//			String^ convertToMusicXML() {
//				return "<type>" + _name + "</type>";
//			}
//	};







}
}

#endif
