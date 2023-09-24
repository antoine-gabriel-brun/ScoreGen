#pragma region head

	#pragma once

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Drawing;
	using namespace System::Drawing::Drawing2D;

	#include "5. Block.h"
	#include "6. Clef.h"

#pragma endregion

namespace ScoreGenLib {
namespace Music {
	
	// Partition complète (non mutable : pour manipuler, utiliser Block)
	public ref class Score {

		// Champs
		private:
			int              _nbParts;
			int              _nbBars;
			Block^           _block;
			array<Clef^, 1>^ _clefs;

		// Constructeur
		public:
			Score(Block^ block, array<Clef^, 1>^ clefs) :
				_nbParts(block->nbParts),
				_nbBars(block->nbBars),
				_block(block),
				_clefs(copy(clefs))
			{
		
				if (_nbParts == 0) throw gcnew Exception("A score should have at least one part.");
				if (_nbBars  == 0) throw gcnew Exception("A score should have at least one bar.");
				
				if (((Object^)_clefs) == nullptr) throw gcnew ArgumentNullException;
				if (_clefs->Length != _nbParts) throw gcnew Exception("There should be as many clefs as parts.");

			}
			Score(Block^ block) :
				_nbParts(block->nbParts),
				_nbBars (block->nbBars),
				_block  (block)
			{

				if (_nbParts == 0) throw gcnew Exception("A score should have at least one part.");
				if (_nbBars  == 0) throw gcnew Exception("A score should have at least one bar.");
				
				_clefs = bestClefs();
			
			}
		
		// Accesseurs
		public:
			property int nbParts {
				int get() {
					return _nbParts;
				}
			}
			property int nbBars {
				int get() {
					return _nbBars;
				}
			}
			property Block^ block {
				Block^ get() {
					return _block;
				}
			}
			property array<Clef^, 1>^ clefs {
				array<Clef^, 1>^ get() {
					return copy(_clefs);
				}
			}

		// Fonctions publiques
		public:
			array<Clef^, 1>^ bestClefs() {
			
				array<Clef^, 1>^ ans = gcnew array<Clef^, 1>(_nbParts);

				// Déterminer la meilleure clef pour chaque partie
				for (int part = 1; part <= _nbParts; part++) {
					ans[part - 1] = bestClef(part);
				}

				// Retourner
				return ans;

			}

		// Fonctions
		private:
			Clef^ bestClef(int part) {
					
				// Déterminer les notes minimale et maximale
				double minPitch = Double::MaxValue;
				double maxPitch = Double::MinValue;
				bool foundNotes = false;
				{
					for (int bar = 1; bar <= _nbBars; bar++) {
						Group^ contents = _block[part, bar];
						if ((Object^)contents != nullptr) {
							for each (Element^ element in contents->elements) {
								if (element->isNote()) {
									Note^ note = (Note^)element;
									double noteMinPitch = getNoteMinPitch(note);
									double noteMaxPitch = getNoteMaxPitch(note);
									minPitch = Math::Min(minPitch, noteMinPitch);
									maxPitch = Math::Max(maxPitch, noteMaxPitch);
									foundNotes = true;
								} else if (element->isTuplet()) {
									Tuplet^ tuplet = (Tuplet^)element;
									for each (Element^ element_ in tuplet->elements) {
										if (element_->isNote()) {
											Note^ note = (Note^)element_;
											double noteMinPitch = getNoteMinPitch(note);
											double noteMaxPitch = getNoteMaxPitch(note);
											minPitch = Math::Min(minPitch, noteMinPitch);
											maxPitch = Math::Max(maxPitch, noteMaxPitch);
											foundNotes = true;
										}
									}
								}
							}
						}
					}
				}

				// Choisir la meilleure clef
				if (foundNotes) {
					double bad_F_clef = Math::Max(0., maxPitch - 65);
					double bad_G_clef = Math::Max(0., 55 - minPitch);
					if (bad_G_clef <= bad_F_clef) return Clef::G2;
					else                          return Clef::F4;
				} else {
					return Clef::G2;
				}
				
			}
			double getNoteMinPitch(Note^ note) {
			
				double min = Double::MaxValue;

				for each (Pitch^ pitch in note->pitches) {
					double pitch_dbl = pitch->midiValue;
					if (pitch_dbl < min) min = pitch_dbl;
				}

				return min;

			}
			double getNoteMaxPitch(Note^ note) {
			
				double max = Double::MinValue;

				for each (Pitch^ pitch in note->pitches) {
					double pitch_dbl = pitch->midiValue;
					if (pitch_dbl > max) max = pitch_dbl;
				}

				return max;

			}

		// Conversion implicite en 'ScoreAndInfos'
		public:
			operator ScoreAndInfos^();

		// Conversion en texte MusicXML
		public:
			String^ convertToMusicXML() {
				
				// Commencer à suivre les 'tied'
				Element::resetTied();

				// Calculer le texte MusicXML pour chaque signature rythmique
				array<String^, 1>^ timeSignatures_str = gcnew array<String^, 1>(_nbBars); {
					// Créer le texte de chaque signature, en supprimant les signatures répétées
					for (int bar = 1; bar <= _nbBars; bar++) {
						bool show = (bar == 1) || (_block->timeSignature[bar] != _block->timeSignature[bar - 1]);
						timeSignatures_str[bar - 1] = show ? _block->timeSignature[bar]->convertToMusicXML() : "";
					}
				}

				// Préparer la chaîne de caractères
				System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder;

				// Ajouter les lignes de début
				{
					sb->AppendLine("<?xml version=\"1.0\" encoding='UTF-8' standalone='no' ?>");
					sb->AppendLine("<!DOCTYPE score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.0 Partwise//EN\" \"http:/" + "/www.musicxml.org/dtds/partwise.dtd\">");
					// PROV : remettre ça ! // sb->AppendLine("<!-- ========= Generated by composer Antoine Brun [antoinegabrielbrun °at° gmail °dot° com] using his software ScoreGen ========= -->");
					sb->AppendLine("<score-partwise version=\"3.0\">");
				}

				// Générer la liste des parties
				{
					sb->AppendLine("  <part-list>");
					for (int part = 0; part < _nbParts; part++) {
						int partIndex = part + 1;
						sb->AppendLine("    <score-part id=\"P" + partIndex + "\"><part-name /><score-instrument id=\"I" + partIndex + "\"><instrument-name /></score-instrument></score-part>");
					}
					sb->AppendLine("  </part-list>");
				}

				// Générer chaque partie
				for (int part = 1; part <= _nbParts; part++) {
					addPartAsMusicXML(sb, part, timeSignatures_str);
				}

				// Ajouter les lignes de fin
				sb->AppendLine("</score-partwise>");

				// Retourner
				return sb->ToString();

			}
		private:
			void addPartAsMusicXML(System::Text::StringBuilder^ sb, int part, array<String^, 1>^ timeSignatures_str) {

				// Ligne de début de partie
				sb->AppendLine("  <part id=\"P" + part + "\">");
				
				// Générer chaque mesure
				for (int bar = 1; bar <= _nbBars; bar++) {
					addBarAsMusicXML(sb, part, bar, timeSignatures_str);
				}
				
				// Ligne de fin de partie
				sb->AppendLine("  </part>");

			}
			void addBarAsMusicXML(System::Text::StringBuilder^ sb, int part, int bar, array<String^, 1>^ timeSignatures_str) {

				// Lignes de début de mesure (indiquant la clef si début de partition, et éventuellement la signature rhythmique)
				sb->AppendLine("    <measure number=\"" + bar + "\">");
				sb->AppendLine("      " +
					"<attributes>" +
						"<divisions>256</divisions>" +
						timeSignatures_str[bar - 1] +
						((bar == 1) ? _clefs[part - 1]->convertToMusicXML() : "") +
					"</attributes>");
				
				// Générer chaque note (ou une mesure vide)
				Group^ contents = _block[part, bar];
				if (((Object^)contents) == nullptr) {
					int duration_thisTimeSignature = _block->timeSignature[bar]->duration_1024th;
					Rest^ wholeBarRest = Rest::wholeBarRest(duration_thisTimeSignature);
					sb->AppendLine("      " + wholeBarRest->convertToMusicXML(part));
				} else {
					for each (Element^ element in contents->elements) {
						sb->AppendLine("      " + element->convertToMusicXML(part));
					}
				}
				
				// Ligne de fin de mesure
				sb->AppendLine("    </measure>");
				
			}

	};
	public ref struct ScoreAndInfos {
	
		public:
			Score ^ score;
			Bitmap^ image;
			String^ comments;

		public:
			ScoreAndInfos(Score^ score, Bitmap^ image, String^ comments) :
				score(score),
				image(image),
				comments(comments)
			{
			
				if (((Object^)score) == nullptr) throw gcnew ArgumentNullException;
			
			}
			ScoreAndInfos(Score^ score, Bitmap^ image                  ) :
				score(score),
				image(image),
				comments("")
			{
			
				if (((Object^)score) == nullptr) throw gcnew ArgumentNullException;
			
			}
			ScoreAndInfos(Score^ score,                String^ comments) :
				score(score),
				image(nullptr),
				comments(comments)
			{
			
				if (((Object^)score) == nullptr) throw gcnew ArgumentNullException;
			
			}
			ScoreAndInfos(Score^ score                                 ) :
				score(score),
				image(nullptr),
				comments("")
			{
			
				if (((Object^)score) == nullptr) throw gcnew ArgumentNullException;
			
			}

	};

}
}
