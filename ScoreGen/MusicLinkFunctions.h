#pragma region head

	#pragma once

	using namespace System;

#pragma endregion

namespace ScoreGenLib {
namespace Music {

	// Opérateurs de conversion
	RhythmFigure::operator Rhythm^() {

		return gcnew Rhythm(this);

	}
	Articulation::operator NoteDecorations^() {
	
		return gcnew NoteDecorations(this);

	}
	Group::operator Block^() {
	
		Block^ ans = gcnew Block;

		ans->addBar(gcnew Signature(_duration_1024th, nullptr));
		ans->addPart();

		ans[1, 1] = this;

		return ans;

	}
	Element::operator Group^() {
	
		return gcnew Group(this);

	}
	Element::operator Block^() {
	
		return (Group^)this;

	}
	Block::operator Score^() {
	
		return gcnew Score(this);

	}
	Block::operator ScoreAndInfos^() {
	
		return gcnew ScoreAndInfos(gcnew Score(this));

	}
	Score::operator ScoreAndInfos^() {
	
		return gcnew ScoreAndInfos(this);

	}

	// Autres opérateurs
	Group^ Element::operator + (Element^ element1, Element^ element2) {
	
		return gcnew Group(newArray(element1, element2));

	}
	Group^ Element::operator * (Element^ element, int nbRepetitions) {
	
		return gcnew Group(fill(element, nbRepetitions));

	}

	// Conversion vers MusicXML
	String^ Rhythm::convertToMusicXML(int instrumentIndex, bool tiedFromPrev, bool tiedToNext, TupletRatio^ tupletRatio) {
		
		// Durée
		String^ dur_str; {
			int dur = _isGrace ? (gcnew Rhythm(_rhythmFigure, _dots))->_duration_1024th : duration_1024th;
			dur_str = "<duration>" + dur + "</duration>"; // fonctionne même si (_specialDuration == true)
		}

		// Liaisons de note
		String^ tied_str  = (tiedFromPrev ? "<tie type=\"stop\" />"  : "");
				tied_str += (tiedToNext   ? "<tie type=\"start\" />" : "");

		// Instrument
		String^ instr_str = "<instrument id=\"I" + instrumentIndex + "\" />";

		// Figure rythmique
		String^ type_str = rhythmFigure->convertToMusicXML();

		// Stem (direction de la hampe)
		String^ stem_str = _isGrace ? "<stem>up</stem>" : "";

		// Points
		String^ dots_str = ""; for (int i = 0; i < _dots; i++) dots_str += "<dot />";

		// TupletRatio
		String^ uplet_str = (tupletRatio == nullptr ? "" : tupletRatio->convertToMusicXML());

		// Retourner
		return dur_str + tied_str + instr_str + type_str + stem_str + dots_str + uplet_str;

	}
	String^ Note::convertToMusicXML(int instrumentIndex) {
	
		// Extraire les notations s'appliquant à la note (tous types de liaisons et d'articulations)
		String^ notations = _decorations->convertToMusicXML(lastNoteWasTied);

		// Préparer la chaîne de caractères
		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder;

		// Avant le silence, insérer les textes éventuels (ceux qui ne sont pas dans la note) ; garder les autres
		String^ wordsInNote = "";
		for each (Words::Words^ w in _words) {
			String^ xml = w->convertToMusicXML();
			if (w->isInNote()) wordsInNote += xml;
			else               sb->Append(xml + "\r\n        ");
		}

		// Ecrire la note comme une succession de notes MusicXML (accord)
		{
			int nbPitches = _pitches->Length;
			for (int i = 0; i < nbPitches; i++) {
				Pitch^ pitch = _pitches[i];
				bool isFirst = (i == 0);
				String ^pitch_str, ^rhythm_str; {
					pitch_str = pitch->convertToMusicXML();
					bool parentUpletIsNull = ((Object^)_parentTuplet == nullptr);
					TupletRatio^ ratio = parentUpletIsNull ? nullptr : _parentTuplet->ratio;
					rhythm_str = _rhythm->convertToMusicXML(instrumentIndex, lastNoteWasTied, _decorations->tied, ratio);
					// Supprimer <duration>...</duration> pour le premier élément d'un accord d'appoggiature
					if (i == 0 && _isGrace) {
						int index1 = rhythm_str->IndexOf("<duration>");
						int index2 = rhythm_str->IndexOf("</duration>");
						if (index1 > -1 && index2 > -1 && index2 > index1) {
							rhythm_str = rhythm_str->Substring(0, index1) + rhythm_str->Substring(index2 + 11);
						}
					}
				}
				sb->Append(
					(isFirst ? "" : "\r\n        ") +
					"<note>" +
						(isFirst && _isGrace ? "<grace />" : "") +
						(isFirst ? "" : "<chord />") +
						pitch_str +
						rhythm_str +
						notations +
						(isFirst ? wordsInNote : "") +
					"</note>"
				);
			}
		}

		// Calculer le prochain 'tied'
		lastNoteWasTied = decorations->tied;

		// Retourner
		return sb->ToString();

	}
	String^ Rest::convertToMusicXML(int instrumentIndex) {

		// Préparer la chaîne de caractères
		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder;

		// Avant le silence, insérer les textes éventuels (ceux qui ne sont pas dans la note) ; garder les autres
		String^ wordsInNote = "";
		for each (Words::Words^ w in _words) {
			String^ xml = w->convertToMusicXML();
			if (w->isInNote()) wordsInNote += xml;
			else               sb->Append(xml + "\r\n        ");
		}

		// Rythme
		String^ rhythm_str; {
			rhythm_str = _rhythm->convertToMusicXML(instrumentIndex, false, false, _parentTuplet == nullptr ? nullptr : _parentTuplet->ratio);
			// Supprimer <duration>...</duration> pour le premier élément d'un accord d'appoggiature
			if (_isGrace) {
				int index1 = rhythm_str->IndexOf("<duration>");
				int index2 = rhythm_str->IndexOf("</duration>");
				if (index1 > -1 && index2 > -1 && index2 > index1) {
					rhythm_str = rhythm_str->Substring(0, index1) + rhythm_str->Substring(index2 + 11);
				}
			}
		}

		// Ecrire le silence
		sb->Append(
			"\r\n        " +
			"<note>" +
				(_isGrace ? "<grace />" : "") +
				"<rest />" +
				rhythm_str +
				wordsInNote +
			"</note>"
		);

		// Rompre les 'tied'
		lastNoteWasTied = false;

		// Retourner
		return sb->ToString();

	}

	// Autres fonctions
	BarIndexAndTime^ BarIndexAndTime::getInBlock(Block^ block, int time_1024th) {
	
		if (time_1024th < 0) throw exc_positiveTime;

		// Parcourir les mesures en incrémentant le temps
		int timeCurrentBar_1024th = 0;
		int timeNextBar_1024th;
		for (int bar = 1; bar <= block->nbBars; bar++) {
			// Déterminer la durée de la mesure et l'instant de la mesure suivante
			int barLength_1024th = block->timeSignature[bar]->duration_1024th;
			timeNextBar_1024th = timeCurrentBar_1024th + barLength_1024th;
			// Est-on dans la mesure qui contient l'instant demandé ?
			if (time_1024th < timeNextBar_1024th) {
				// Retourner
				return gcnew BarIndexAndTime(bar, time_1024th - timeCurrentBar_1024th);
			}
			// Incrémenter le temps courant
			timeCurrentBar_1024th = timeNextBar_1024th;
		}

		// Retourner (après la dernière mesure)
		return gcnew BarIndexAndTime(block->nbBars + 1, time_1024th - timeCurrentBar_1024th);

	}
	void Block::addBars(int nb, String^ timeSignature) {
	
		addBars(nb, qSignature(timeSignature));

	}
	void Block::addBars(int nb, array<String^, 1>^ timeSignatures) {

		int len = timeSignatures->Length;
	
		array<Signature^, 1>^ timeSignatures_convert = gcnew array<Signature^, 1>(len);
		for (int i = 0; i < len; i++) {
			timeSignatures_convert[i] = qSignature(timeSignatures[i]);
		}
	
		addBars(nb, timeSignatures_convert);

	}
	void Block::addBar(String^ timeSignature) {
	
		addBar(qSignature(timeSignature));

	}

}
}
