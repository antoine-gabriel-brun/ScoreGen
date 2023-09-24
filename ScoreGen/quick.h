#pragma region head

	#pragma once

	using namespace System;

	#include "6. Score.h"

#pragma endregion

// Ce fichier contient des fonctions pour créer rapidement des objets grâce à leur représentation textuelle.
namespace ScoreGenLib {
namespace Music {
	
	// Utilitaires privés
	namespace {
		private ref class ToolsForQuick abstract {
		
			// Pour 'qPitch'
			public:
				ref class PossibleAlteration {
				
					private:
						String^     _qDescription;
						Alteration^ _value;

					public:
						PossibleAlteration(String^ qDescription, Alteration^ value) :
							_qDescription(qDescription),
							_value       (value       )
						{ }

					public:
						property String^ qDescription {
							String^ get() {
								return _qDescription;
							}
						}
						property Alteration^ value {
							Alteration^ get() {
								return _value;
							}
						}

				};
				static array<PossibleAlteration^, 1>^ possibleAlterations = gcnew array<PossibleAlteration^, 1> {
					// Classer impérativement du plus long au plus court
					gcnew PossibleAlteration("#(#)", gcnew Alteration( 1.5)),
					gcnew PossibleAlteration("b(b)", gcnew Alteration(-1.5)),
					gcnew PossibleAlteration("(#)" , gcnew Alteration( 0.5)),
					gcnew PossibleAlteration("(b)" , gcnew Alteration(-0.5)),
					gcnew PossibleAlteration("##"  , gcnew Alteration( 2  )),
					gcnew PossibleAlteration("bb"  , gcnew Alteration(-2  )),
					gcnew PossibleAlteration("#"   , gcnew Alteration( 1  )),
					gcnew PossibleAlteration("b"   , gcnew Alteration(-1  )),
					gcnew PossibleAlteration(""    , gcnew Alteration( 0  ))
				};
		
			// Pour 'qRhythmFigure'
			public:
				ref class PossibleRhythmFigure {
				
					private:
						String^       _qDescription;
						RhythmFigure^ _value;

					public:
						PossibleRhythmFigure(String^ qDescription, RhythmFigure^ value) :
							_qDescription(qDescription),
							_value       (value       )
						{ }

					public:
						property String^ qDescription {
							String^ get() {
								return _qDescription;
							}
						}
						property RhythmFigure^ value {
							RhythmFigure^ get() {
								return _value;
							}
						}

				};
				static array<PossibleRhythmFigure^, 1>^ possibleRhythmFigures = gcnew array<PossibleRhythmFigure^, 1> {
					gcnew PossibleRhythmFigure("256th"  , RhythmFigure::_256th  ),
					gcnew PossibleRhythmFigure("256"    , RhythmFigure::_256th  ),
					gcnew PossibleRhythmFigure("128th"  , RhythmFigure::_128th  ),
					gcnew PossibleRhythmFigure("128"    , RhythmFigure::_128th  ),
					gcnew PossibleRhythmFigure("64th"   , RhythmFigure::_64th   ),
					gcnew PossibleRhythmFigure("64"     , RhythmFigure::_64th   ),
					gcnew PossibleRhythmFigure("32nd"   , RhythmFigure::_32nd   ),
					gcnew PossibleRhythmFigure("32"     , RhythmFigure::_32nd   ),
					gcnew PossibleRhythmFigure("16th"   , RhythmFigure::_16th   ),
					gcnew PossibleRhythmFigure("16"     , RhythmFigure::_16th   ),
					gcnew PossibleRhythmFigure("eighth" , RhythmFigure::_eighth ),
					gcnew PossibleRhythmFigure("e"      , RhythmFigure::_eighth ),
					gcnew PossibleRhythmFigure("quarter", RhythmFigure::_quarter),
					gcnew PossibleRhythmFigure("q"      , RhythmFigure::_quarter),
					gcnew PossibleRhythmFigure("half"   , RhythmFigure::_half   ),
					gcnew PossibleRhythmFigure("h"      , RhythmFigure::_half   ),
					gcnew PossibleRhythmFigure("whole"  , RhythmFigure::_whole  ),
					gcnew PossibleRhythmFigure("w"      , RhythmFigure::_whole  ),
					gcnew PossibleRhythmFigure("breve"  , RhythmFigure::_breve  ),
					gcnew PossibleRhythmFigure("b"      , RhythmFigure::_breve  ),
					gcnew PossibleRhythmFigure("long"   , RhythmFigure::_long   ),
					gcnew PossibleRhythmFigure("l"      , RhythmFigure::_long   )
				};

			// Pour 'qElement' et 'qElements'
			public:
				// String split intelligent : séparer aux espaces, mais en gardant groupés les [] et les <>
				static array<String^, 1>^ cleverStringSplit(String^ string) {
						
					// Lister les points de coupure
					array<int, 1>^ cuts;
					int nbCuts;
					{
						List<int>^ cuts_lst = gcnew List<int>;
						int openedBrackets1 = 0;
						int openedBrackets2 = 0;
						int pos = 0;
						cuts_lst->Add(-1);
						for each (wchar_t c in string) {
							if      (c == L'[') openedBrackets1++;
							else if (c == L']') openedBrackets1--;
							else if (c == L'<') openedBrackets2++;
							else if (c == L'>') openedBrackets2--;
							else if (c == L' ') if (openedBrackets1 == 0 && openedBrackets2 == 0) cuts_lst->Add(pos);
							pos++;
						}
						cuts_lst->Add(pos);
						cuts = cuts_lst->ToArray();
						nbCuts = cuts->Length;
					}
					
					// Etablir la liste (et supprimer du même coup les entrées vides)
					List<String^>^ ans = gcnew List<String^>; {
						for (int i = 1; i < nbCuts; i++) {
							int posCut1 = cuts[i - 1];
							int posCut2 = cuts[i    ];
							String^ entry = string->Substring(posCut1 + 1, posCut2 - posCut1 - 1);
							if (entry != "") ans->Add(entry);
						}
					}
				
					// Retourner
					return ans->ToArray();
				
				}

			// Pour 'qBlock'
			public:
				static array<array<int, 1>^, 1>^ getRangesFramedBySquareBrackets(String^ s) {
					
					List<array<int, 1>^>^ ranges = gcnew List<array<int, 1>^>;

					int pos = 0;
					int posOpen;
					int level = 0;
					for each (wchar_t c in s) {
						if (c == L'[') {
							if (level == 0) posOpen = pos + 1;
							level++;
						} else if (c == L']') {
							level--;
							if (level < 0) throw gcnew Exception("Unexpected closing bracket.");
							else if (level == 0) ranges->Add(gcnew array<int, 1>{posOpen, pos - 1});
						}
						pos++;
					}
					if (level > 0) throw gcnew Exception("Unclosed bracket.");

					return ranges->ToArray();

				}
				static bool thereAreCharsOtherThanSpacesBetweenBracketRanges(String^ s, array<array<int, 1>^, 1>^ ranges) {
				
					int nbRanges = ranges->Length;
					int strLen = s->Length;

					for (int i = 0; i <= nbRanges; i++) {
						int beg = (i == 0) ? 0 : ranges[i - 1][1] + 2;
						int end = (i == nbRanges) ? strLen - 1 : ranges[i][0] - 2;
						String^ between = s->Substring(beg, end - beg + 1);
						if (between->Replace(" ", "")->Replace("\t", "")->Replace("\r", "")->Replace("\n", "") != "") return true;
					}

					return false;
				
				}

		};
	};

	// Pitch : 64, 50.5, Fbb4, *, 1/3, 4/25, Cb(b)2, Cb-1, B(b)10, D5, D(#)7, A#0, E#(#)3, G##4
	Pitch^ qPitch(String^ qDescription) {

		// Erreurs
		if (qDescription->Contains(" ") || qDescription->Contains("\t") || qDescription->Contains("\r") || qDescription->Contains("\n")) throw gcnew Exception("A pitch qDescription shouldn't contain any space char.");
		
		// Caractère '*' : pitch non spécifié
		if (qDescription == "*") return gcnew Pitch;

		// Ratio du type "2/5" : pitch de percussion
		{
			int index_first = qDescription->IndexOf(L'/');
			if (index_first != -1) {
				int index_last = qDescription->LastIndexOf(L'/');
				if (index_first != index_last) throw gcnew Exception("A pitch qDescription shouldn't contain the char '/' twice.");
				String^  left_str = qDescription->Substring(0, index_first);
				String^ right_str = qDescription->Substring(index_first + 1);
				int left, right;
				if (!Int32::TryParse(left_str , left )) throw gcnew Exception("The part before the char '/' should be an integer.");
				if (!Int32::TryParse(right_str, right)) throw gcnew Exception("The part after the char '/' should be an integer.");
				if (left <= 0 || right <= 0) throw gcnew Exception("The two integers should be positive.");
				if (left > right) throw gcnew Exception("The first integer should be between 1 and the second integer.");
				return gcnew Pitch(left, right);
			}
		}

		// S'il s'agit d'un nombre, l'interpréter comme une valeur MIDI (éventuellement fractionnaire)
		{
			double dbl;
			if (Double::TryParse(qDescription, dbl) || Double::TryParse(qDescription->Replace(".", ","), dbl) || Double::TryParse(qDescription->Replace(",", "."), dbl)) {
				return gcnew Pitch(dbl);
			}
		}

		// Lister les caractères
		array<wchar_t, 1>^ chars = qDescription->ToCharArray();
		int nb = chars->Length;

		// Extraire le nom de la note (un caractère entre A et G, aussi accepté en minuscule)
		char step; {
			if (nb == 0) throw gcnew Exception("If not numeric and not '*', a pitch qDescription should have at least 2 chars.");
			wchar_t wstep = chars[0];
			if (wstep >= L'a' && wstep <= L'g') wstep = wstep + L'A' - L'a';
			if (!(wstep >= L'A' && wstep <= L'G')) throw gcnew Exception("If not numeric and not '*', a pitch qDescription should begin with a char in the range a - g or A - G.");
			step = (char)wstep;
		}

		// Extraire l'altération éventuelle parmi les valeurs possibles
		Alteration^ alteration;
		int posAfterAlts;
		{
			for each (ToolsForQuick::PossibleAlteration^ possibleAlteration in ToolsForQuick::possibleAlterations) {
				String^ beginIs = (wchar_t)step + possibleAlteration->qDescription;
				if (qDescription->ToUpper()->StartsWith(beginIs->ToUpper())) {
					alteration = possibleAlteration->value;
					posAfterAlts = possibleAlteration->qDescription->Length + 1;
					break; // ce 'break' finit toujours par se produire, car l'altération '' est possible
				}
			}
		}

		// Extraire l'octave
		int octave; {
			String^ oct_str = gcnew String(chars, posAfterAlts, nb - posAfterAlts);
			int octStr_len = oct_str->Length;
			if (octStr_len == 0) throw gcnew Exception("After the step and the optional alterations, a pitch qDescription should contain the octave number.");
			String^ err = "After the step and the optional alterations, a pitch qDescription should only contain the octave number, possibly preceded by the '-' symbol for negative octaves.";
			for (int i = 0; i < octStr_len; i++) {
				wchar_t c = oct_str[i];
				if (!(c >= L'0' && c <= L'9')) {
					if (!(c == L'-' && i == 0)) throw gcnew Exception(err);
				}
			}
			if (!Int32::TryParse(oct_str, octave)) throw gcnew Exception(err);
		}

		// Créer
		return gcnew Pitch(step, alteration, octave);

	}

	// RhythmFigure : q, whole, 128th, 64
	RhythmFigure^ qRhythmFigure(String^ qDescription) {
		
		qDescription = qDescription->ToLower();

		for each (ToolsForQuick::PossibleRhythmFigure^ possibleRhythmFigures in ToolsForQuick::possibleRhythmFigures) {
			if (qDescription == possibleRhythmFigures->qDescription) {
				return possibleRhythmFigures->value;
			}
		}

		throw gcnew Exception("To create a 'RhythmFigure', the only qDescriptions allowed are '256th' or '256', '128th' or '128', '64th' or '64', '32nd' or '32', '16th' or '16', 'eighth' or 'e', 'quarter' or 'q', 'half' or 'h', 'whole' or 'w', 'breve' or 'b', and 'long' or 'l' (and all corresponding upper-case descriptions).");

	}

	// Rhythm : q, eighth., 128...., 32nd
	Rhythm^ qRhythm(String^ qDescription) {
		
		// Compter les points finaux
		int nbDots; {
			nbDots = 0;
			for each (wchar_t c in qDescription) {
				if (c == L'.') nbDots++;
				else           nbDots = 0;
			}
		}

		// Extraire la sous-chaîne censée indiquer la figure rythmique
		String^ rhythmFigure_str = qDescription->Substring(0, qDescription->Length - nbDots);

		// Déterminer la figure rhythmique
		RhythmFigure^ rhythmFigure = qRhythmFigure(rhythmFigure_str);

		// Retourner
		return gcnew Rhythm(rhythmFigure, nbDots);

	}

	// TupletRatio : 3:2, q:5:4, 128th:2:3, 128:2:3
	TupletRatio^ qTupletRatio(String^ qDescription) {
		
		// Récupérer les 3 chaînes séparées par des symboles ':'
		array<String^, 1>^ values = qDescription->Split(gcnew array<String^, 1> { ":" }, StringSplitOptions::None);
		int nbValues = values->Length;
		if (nbValues != 3) throw gcnew Exception("A Tuplet qDescription without 'baseIfUnspecified' should contain 3 elements delimited by ':'.");

		// Retourner
		return gcnew TupletRatio(
			qRhythm     (values[0]),
			Int32::Parse(values[1]),
			Int32::Parse(values[2])
		);

	}
	TupletRatio^ qTupletRatio(String^ qDescription, Rhythm^ baseIfUnspecified) {
		
		// Récupérer les 2 ou 3 chaînes séparées par des symboles ':'
		array<String^, 1>^ values = qDescription->Split(gcnew array<String^, 1> { ":" }, StringSplitOptions::None);
		int nbValues = values->Length;

		// Retourner, suivant le nombre d'éléments (2 ou 3)
		if (nbValues == 2) {
			return gcnew TupletRatio(
				baseIfUnspecified,
				Int32::Parse(values[0]),
				Int32::Parse(values[1])
			);
		} else if (nbValues == 3) {
			return gcnew TupletRatio(
				qRhythm(values[0]),
				Int32::Parse(values[1]),
				Int32::Parse(values[2])
			);
		} else {
			throw gcnew Exception("A Tuplet qDescription should contain 2 or 3 elements delimited by ':'.");
		}

	}

	// Words : .normal, !nuance, :italique, °tempo, #commentaire, *paroles
	Words::Words^ qWords(String^ qDescription) {
		
		Exception^ exc = gcnew Exception("The first char of the Words qDescription should indicate the words type : '!', ':', '.', '°', '#', or '*'.");

		if (qDescription == "") throw exc;
		
		wchar_t type     = qDescription[0];
		String^ contents = qDescription->Substring(1);
		
		if      (type == L'.') return gcnew Words::Normal (contents);
		else if (type == L'!') return gcnew Words::Dynamic(contents);
		else if (type == L':') return gcnew Words::Italic (contents);
		else if (type == L'°') return gcnew Words::Tempo  (contents);
		else if (type == L'#') return gcnew Words::Comment(contents);
		else if (type == L'*') return gcnew Words::Lyrics (contents);
		else                   throw exc;

	}

	// Articulation(s) : @-'^
	Articulation^ qArticulation(wchar_t qDescription) {
		
		if      (qDescription == L'@' ) return Articulation::accent      ;
		else if (qDescription == L'-' ) return Articulation::tenuto      ;
		else if (qDescription == L'\'') return Articulation::staccato    ;
		else if (qDescription == L'^' ) return Articulation::strongAccent;
		else                           throw gcnew Exception("An Articulation qDescription can only be '@' (accent), '-' (tenuto), ''' (staccato), or '^' (strong accent).");

	}
	array<Articulation^, 1>^ qArticulations(String^ qDescription) {
		
		bool accent       = false;
		bool tenuto       = false;
		bool staccato     = false;
		bool strongAccent = false;

		Exception^ exc = gcnew Exception("Articulations qDescriptions can only contain '@' (accent), '-' (tenuto), ''' (staccato), or '^' (strong accent), each once at the most.");

		for each (wchar_t c in qDescription) {
			if      (c == L'@' ) { if (accent      ) throw exc; else accent       = true; }
			else if (c == L'-' ) { if (tenuto      ) throw exc; else tenuto       = true; }
			else if (c == L'\'') { if (staccato    ) throw exc; else staccato     = true; }
			else if (c == L'^' ) { if (strongAccent) throw exc; else strongAccent = true; }
			else                 { throw exc;                                             }
		}

		List<Articulation^>^ ans = gcnew List<Articulation^>;
		if (accent      ) ans->Add(Articulation::accent      );
		if (tenuto      ) ans->Add(Articulation::tenuto      );
		if (staccato    ) ans->Add(Articulation::staccato    );
		if (strongAccent) ans->Add(Articulation::strongAccent);

		return ans->ToArray();

	}
	
	// NoteDecorations : @-'^_()
	NoteDecorations^ qNoteDecorations(String^ qDescription) {
		
		bool accent       = false;
		bool tenuto       = false;
		bool staccato     = false;
		bool strongAccent = false;
		bool tied         = false;
		bool slurBeg      = false;
		bool slurEnd      = false;

		Exception^ exc = gcnew Exception("NoteDecorations qDescriptions can only contain '@' (accent), '-' (tenuto), ''' (staccato), '^' (strong accent), '_' (tied), '(' (slur begin), or ')' (slur end), each once at the most.");

		for each (wchar_t c in qDescription) {
			if      (c == L'@' ) { if (accent      ) throw exc; else accent       = true; }
			else if (c == L'-' ) { if (tenuto      ) throw exc; else tenuto       = true; }
			else if (c == L'\'') { if (staccato    ) throw exc; else staccato     = true; }
			else if (c == L'^' ) { if (strongAccent) throw exc; else strongAccent = true; }
			else if (c == L'_' ) { if (tied        ) throw exc; else tied         = true; }
			else if (c == L'(' ) { if (slurBeg     ) throw exc; else slurBeg      = true; }
			else if (c == L')' ) { if (slurEnd     ) throw exc; else slurEnd      = true; }
			else                 { throw exc;                                             }
		}

		List<Articulation^>^ ans = gcnew List<Articulation^>;
		if (accent      ) ans->Add(Articulation::accent      );
		if (tenuto      ) ans->Add(Articulation::tenuto      );
		if (staccato    ) ans->Add(Articulation::staccato    );
		if (strongAccent) ans->Add(Articulation::strongAccent);

		return gcnew NoteDecorations(ans->ToArray(), tied, slurBeg, slurEnd);

	}

	// Element : voir exemples dans le corps de la fonction
	Element^ qElement(String^ qDescription) {
		
		// Exemples de qDescriptions valides, avec leur explication
		{
			//	q|64                   = noire de pitch MIDI 64                                                    
			//	q|64.5<!pp>            = noire de pitch MIDI 64.5, pianissimo                                      
			//	e...<:dolce>           = demi-soupir triplement pointé indiqué "dolce"                             
			//	h_|Cb-1<AH AH>         = blanche liée à la note suivante, de pitch Cb-1, indiquée "AH AH"          
			//	16th(|D5               = double-croche démarrant une liaison de phrasé, de pitch D5                
			//	16th_)|D(#)7           = double-croche terminant une liaison de phrasé, de pitch D(#)7             
			//	w.                     = pause pointée                                                             
			//	h._|E#(#)3             = blanche pointée liée à la note suivante, de pitch E#(#)3                  
			//	q|G##4                 = noire de pitch G##4                                                       
			//	e,|C4                  = appoggiature de croche de pitch C4                                        
			//	e@-'^|C4               = croche avec un accent, un louré, un staccato et une attaque               
			//	[3:2 e|G5 e|G5 e|G5]   = trois croches de triolet, de pitch G5                                     
			//	[q:5:4 h|G5 h|G5 q|G5] = deux blanches et une noire croche dans un quintolet de noires, de pitch G5
		}

		// Erreurs
		if (qDescription == "") throw gcnew Exception("An Element qDescription shouldn't be void.");

		// Directement enlever les parties finales éventuelles entre <chevrons> (textes).
		array<Words::Words^, 1>^ words; {
			// Tant que la chaîne 'name' se termine par un chevron fermant, extraire le <contenu>, et amputer 'name'
			List<Words::Words^>^ words_lst; {
				String^ err = "Symbols '<' and '>' should be corresponding, not nested, and only at the end.";
				words_lst = gcnew List<Words::Words^>;
				while (qDescription->EndsWith(">")) {
					// Isoler l'élément entre <chevrons>
					int pos_open = qDescription->LastIndexOf("<");
					if (pos_open == -1) throw gcnew Exception(err);
					String^ add_str = qDescription->Substring(pos_open + 1, qDescription->Length - pos_open - 2);
					if (add_str->Contains(">")) throw gcnew Exception(err);
					if (add_str->Length < 2) throw gcnew Exception("The contents of <> should be at least 2 chars.");
					Words::Words^ add = qWords(add_str);
					// Transférer l'élément de 'name' vers 'texts'
					words_lst->Add(add);
					qDescription = qDescription->Substring(0, pos_open);
				};
			}
			// Vérifier qu'il reste quelque chose, et que ce quelque chose ne contient ni chevrons ni crochets
			if (qDescription == "") throw gcnew Exception("There should be something before the first <text>.");
			bool seemsToBeUplet = qDescription->Contains("[") || qDescription->Contains("]");
			if (!seemsToBeUplet && (qDescription->Contains("<") || qDescription->Contains(">"))) throw gcnew Exception("All the <texts> should be at the end.");
			if (seemsToBeUplet && words_lst->Count > 0) throw gcnew Exception("In an Tuplet, the <texts> should be attached directly to one of the notes or one of the rests.");
			// Enregistrer, en inversant l'ordre
			words_lst->Reverse();
			words = words_lst->ToArray();
		}
	
		// Si on a affaire à un Tuplet entre crochets, le traiter comme tel et retourner
		if (qDescription->StartsWith("[") && qDescription->EndsWith("]")) {
			qDescription = qDescription->Substring(1, qDescription->Length - 2);
			array<String^, 1>^ subElements = ToolsForQuick::cleverStringSplit(qDescription);
			if (subElements->Length < 2) throw gcnew Exception("A Tuplet qDescription should have at least two elements delimited by spaces : a ratio and at least one note or rest.");
			// Parser les valeurs (indices 1 à la fin du tableau)
			int nbElements = subElements->Length - 1;
			array<NoteOrRest^, 1>^ notesAndRests = gcnew array<NoteOrRest^, 1>(nbElements);
			for (int i = 1; i <= nbElements; i++) {
				Element^ e = qElement(subElements[i]);
				if (e->isTuplet()) throw gcnew Exception("Nested tuplets are not supported.");
				else notesAndRests[i - 1] = (NoteOrRest^)e;
			}
			// Parser le rapport (indice 0)
			TupletRatio^ ratio = qTupletRatio(subElements[0], notesAndRests[0]->rhythm);
			// Retourner le Tuplet
			return gcnew Tuplet(ratio, notesAndRests);
		}
	
		// Extraire la partie "durée" et les parties optionnelles "decoration" et "pitches"
		String^            str_duration;
		String^            str_decoration;
		array<String^, 1>^ str_pitches;
		{
			// Séparer entre les symboles '|' : le premier élément devient 'str_duration_decoration', les autres 'str_pitches'.
			String^ str_duration_decoration; {
				array<String^, 1>^ elements = qDescription->Split(gcnew array<String^, 1>{"|"}, StringSplitOptions::None);
				str_duration_decoration = elements[0];
				str_pitches = gcnew array<String^, 1>(elements->Length - 1);
				for (int i = 1; i < elements->Length; i++) str_pitches[i - 1] = elements[i];
			}
			// Séparer en (durée) + (décoration)
			int index1 = str_duration_decoration->IndexOf(L'_');
			int index2 = str_duration_decoration->IndexOf(L'(');
			int index3 = str_duration_decoration->IndexOf(L')');
			int index4 = str_duration_decoration->IndexOf(L',');
			int index5 = str_duration_decoration->IndexOf(L'@');
			int index6 = str_duration_decoration->IndexOf(L'-');
			int index7 = str_duration_decoration->IndexOf(L'\'');
			int index8 = str_duration_decoration->IndexOf(L'^');
			{
				if (index1 == -1 && index2 == -1 && index3 == -1 && index4 == -1 && index5 == -1 && index6 == -1 && index7 == -1 && index8 == -1) {
					str_duration = str_duration_decoration;
					str_decoration = "";
				} else {
					if (index1 == -1) index1 = str_duration_decoration->Length + 10;
					if (index2 == -1) index2 = str_duration_decoration->Length + 10;
					if (index3 == -1) index3 = str_duration_decoration->Length + 10;
					if (index4 == -1) index4 = str_duration_decoration->Length + 10;
					if (index5 == -1) index5 = str_duration_decoration->Length + 10;
					if (index6 == -1) index6 = str_duration_decoration->Length + 10;
					if (index7 == -1) index7 = str_duration_decoration->Length + 10;
					if (index8 == -1) index8 = str_duration_decoration->Length + 10;
					int posDecoration = Math::Min(
							Math::Min(Math::Min(index1, index2), Math::Min(index3, index4)),
							Math::Min(Math::Min(index5, index6), Math::Min(index7, index8))
					);
					str_duration = str_duration_decoration->Substring(0, posDecoration);
					str_decoration = str_duration_decoration->Substring(posDecoration);
				}
			}
		}
	
		// Analyser 'str_duration' : une valeur rhythmique, puis 0 à 4 fois le caractère '.'
		Rhythm^ rhythm = qRhythm(str_duration);
	
		// Analyser 'str_decoration' : 0 ou 1 fois chacun des caractères _(),@-'^
		bool tied, slurBeg, slurEnd, grace, accent, tenuto, staccato, strong; {
			String^ str = str_decoration;
			{
				int count_before = str->Length;
				str = str->Replace("_", "");
				int count_after = str->Length;
				tied = (count_after < count_before);
				if (count_after < count_before - 1) throw gcnew Exception("The symbols _(),@-'^ can be present 0 or 1 time.");
			}
			{
				int count_before = str->Length;
				str = str->Replace("(", "");
				int count_after = str->Length;
				slurBeg = (count_after < count_before);
				if (count_after < count_before - 1) throw gcnew Exception("The symbols _(),@-'^ can be present 0 or 1 time.");
			}
			{
				int count_before = str->Length;
				str = str->Replace(")", "");
				int count_after = str->Length;
				slurEnd = (count_after < count_before);
				if (count_after < count_before - 1) throw gcnew Exception("The symbols _(),@-'^ can be present 0 or 1 time.");
			}
			{
				int count_before = str->Length;
				str = str->Replace(",", "");
				int count_after = str->Length;
				grace = (count_after < count_before);
				if (count_after < count_before - 1) throw gcnew Exception("The symbols _(),@-'^ can be present 0 or 1 time.");
			}
			{
				int count_before = str->Length;
				str = str->Replace("@", "");
				int count_after = str->Length;
				accent = (count_after < count_before);
				if (count_after < count_before - 1) throw gcnew Exception("The symbols _(),@-'^ can be present 0 or 1 time.");
			}
			{
				int count_before = str->Length;
				str = str->Replace("-", "");
				int count_after = str->Length;
				tenuto = (count_after < count_before);
				if (count_after < count_before - 1) throw gcnew Exception("The symbols _(),@-'^ can be present 0 or 1 time.");
			}
			{
				int count_before = str->Length;
				str = str->Replace("'", "");
				int count_after = str->Length;
				staccato = (count_after < count_before);
				if (count_after < count_before - 1) throw gcnew Exception("The symbols _(),@-'^ can be present 0 or 1 time.");
			}
			{
				int count_before = str->Length;
				str = str->Replace("^", "");
				int count_after = str->Length;
				strong = (count_after < count_before);
				if (count_after < count_before - 1) throw gcnew Exception("The symbols _(),@-'^ can be present 0 or 1 time.");
			}
			if (str != "") throw gcnew Exception("Between the rhythmic value and the symbol '|', the only correct chars are '_', '(', ')', ',', '@', '-', '\'' and '^'.");
		}
		
		// Analyser 'str_pitches'
		int nbPitches = str_pitches->Length;
		array<Pitch^, 1>^ pitches;
		{
			pitches = gcnew array<Pitch^, 1>(nbPitches);
			for (int i = 0; i < nbPitches; i++) {
				pitches[i] = qPitch(str_pitches[i]);
			}
		}
	
		// Retourner sous la forme d'un silence ou d'une note
		{
			if (nbPitches == 0) {
				// Si aucun pitch n'est précisé, c'est un silence
				if (tied) throw gcnew Exception("A rest shouldn't have a tie to the next note.");
				if (accent || tenuto || staccato || strong) throw gcnew Exception("A rest shouldn't have an articulation.");
				if (grace) {
					if (words->Length > 0) throw gcnew Exception("A grace rest shouldn't have words attached to it.");
					return Rest::graceRest(rhythm);
				} else {
					return gcnew Rest(rhythm, words);
				}
			} else {
				// Sinon, c'est une note
				List<Articulation^>^ articulations = gcnew List<Articulation^>;
				if (accent   ) articulations->Add(Articulation::accent      );
				if (tenuto   ) articulations->Add(Articulation::tenuto      );
				if (staccato ) articulations->Add(Articulation::staccato    );
				if (strong   ) articulations->Add(Articulation::strongAccent);
				NoteDecorations^ decorations = gcnew NoteDecorations(articulations, tied, slurBeg, slurEnd);
				if (grace) {
					if (words->Length > 0) throw gcnew Exception("A grace note shouldn't have words attached to it.");
					return Note::graceNote(rhythm, pitches, decorations);
				} else {
					return gcnew Note(rhythm, pitches, decorations, words);
				}
			}
		}

	}

	// Note, Rest, Tuplet, NoteOrRest : même syntaxe que 'qElement'
	Note^ qNote(String^ qDescription) {
		
		Element^ element = qElement(qDescription);

		if (element->isNote()) return (Note^)element;
		else                   throw gcnew Exception("This qDescription doesn't describe a Note.");

	}
	Rest^ qRest(String^ qDescription) {
		
		Element^ element = qElement(qDescription);

		if (element->isRest()) return (Rest^)element;
		else                   throw gcnew Exception("This qDescription doesn't describe a Rest.");

	}
	Tuplet^ qTuplet(String^ qDescription) {
		
		Element^ element = qElement(qDescription);

		if (element->isTuplet()) return (Tuplet^)element;
		else                     throw gcnew Exception("This qDescription doesn't describe a Tuplet.");

	}
	NoteOrRest^ qNoteOrRest(String^ qDescription) {
		
		Element^ element = qElement(qDescription);

		if (element->isNote() || element->isRest()) return (NoteOrRest^)element;
		else                                        throw gcnew Exception("This qDescription doesn't describe a Note nor a Rest.");

	}

	// Group ou simple suite d'éléments : éléments séparés par des espaces
	array<Element^, 1>^ qElements(String^ qDescription) {

		// Lister les éléments séparés par des espaces, mais en gardant groupés les éléments dans [] ou dans <>
		array<String^, 1>^ elements_str = ToolsForQuick::cleverStringSplit(qDescription);
		
		// Récupérer chaque élément
		List<Element^>^ elements = gcnew List<Element^>;
		for each (String^ element_str in elements_str) {
			elements->Add(qElement(element_str));
		}

		// Retourner
		return elements->ToArray();

	}
	Group^ qGroup(String^ qDescription) {
	
		return gcnew Group(qElements(qDescription));

	}

	// Signature rythmique : 'common', 'C', 'cut', 'C/', '3/4', '1/8', '7/16', '8/1', '2+2+3/8'
	Signature^ qSignature(String^ qDescription) {
		
		qDescription = qDescription->ToLower();
		Exception^ exc = gcnew Exception("Incorrect Signature qDescription. Valid examples are: 'common', 'C', 'cut', 'C/', '3/4', '1/8', '7/16', '8/1', '2+2+3/8'...");

		// Traiter les cas "common" (aussi noté "c") et "cut" (aussi noté "c/")
		if      (qDescription == "common" || qDescription == "c" ) return Signature::common;
		else if (qDescription == "cut"    || qDescription == "c/") return Signature::cut   ;

		// Séparer la partie à gauche et la partie à droite de "/"
		String ^left, ^right; {
			array<String^, 1>^ subStrings = qDescription->Split(gcnew array<wchar_t, 1> { L'/' }, StringSplitOptions::None);
			if (subStrings->Length != 2) throw gcnew Exception("The symbol '/' should come once at the most.");
			left  = subStrings[0];
			right = subStrings[1];
		}

		// Déterminer le ou les numérateurs
		int nbNumerators;
		array<int, 1>^ numerators;
		{
			array<String^, 1>^ numerators_str = left->Split(gcnew array<wchar_t, 1> { L'+' }, StringSplitOptions::None);
			nbNumerators = numerators_str->Length;
			numerators = gcnew array<int, 1>(nbNumerators);
			try {
				for (int i = 0; i < nbNumerators; i++) {
					numerators[i] = Int32::Parse(numerators_str[i]);
				}
			} catch (...) {
				throw exc;
			}
		}

		// Déterminer le dénominateur
		int denominator; {
			try {
				denominator = Int32::Parse(right);
			} catch (...) {
				throw gcnew Exception("The denominator should be an integer.");
			}
		}

		// Retourner
		return gcnew Signature(numerators, denominator);

	}

	// Block complet. Exemple à deux parties et trois mesures : '[2] [C 3/4x2] [[w_|c4] [q|c4 q|d4 q|e4] [h.|f4] | [] [h.|c4] [h.|f3]]'
		// Attention à la difficulté de parsing : les '[' ']' et '|' peuvent faire partie de chaînes de caractères......
		
}
}
