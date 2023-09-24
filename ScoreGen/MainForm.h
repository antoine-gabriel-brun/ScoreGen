	#include "NEW_TEST.hpp" // PROV : teste la nouvelle version de la bibliothèque
#pragma region head

	#pragma once

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Collections::Generic;

	// Inclusion de la bibliothèque
	#include "ScoreGenLib.h"
	using namespace ScoreGenLib;
	using namespace ScoreGenLib::Music;
	using namespace ScoreGenLib::MathForMusic;
	using namespace ScoreGenLib::Generators;

	// Inclusion des morceaux
	#include "AllCreations.h"

#pragma endregion

namespace ScoreGen {
	public ref class MainForm : public System::Windows::Forms::Form {

	// Constructeur, destructeur
	public:
		MainForm() {
		
				// PROV : tests de la nouvelle bibliothèque
				{
					//NEW_testNewLibrary();
					//MessageBox::Show("Pas d'erreur détectée dans le test de la nouvelle bibliothèque.");
					//return;
				}
				//

			// Initialiser les contrôles
			InitializeComponent();

			// Récupérer tous les générateurs de morceaux de type 'Generator'
			array<Type^, 1>^ allPiecesGenerators = ReflectionTools::getChildrenTypesByParentTypeName("Generator", false);
			
			// Créer une instance de chaque générateur dans 'Generators'
			{
				List<Generator^>^ generators_list = gcnew List<Generator^>;
				int nbErr = 0;
				String^ class_err;
				String^ msg_err;
				for each (Type^ pieceGenerator in allPiecesGenerators) {
					try {
						generators_list->Add((Generator^)(Activator::CreateInstance(pieceGenerator)));
					} catch (Exception^ e) {
						nbErr++;
						class_err = pieceGenerator->Name;
						msg_err = e->Message;
					}
				}
				if (nbErr > 0) {
					MessageBox::Show(
						"Nombre de morceaux impossibles à instancier : " + nbErr + "." + "\r\n" +
						"Nom de la dernière classe ayant posé problème : '" + class_err + "'." + "\r\n" +
						"Message d'erreur correspondant : '" + msg_err + "'."
					);
				}
				generators = generators_list->ToArray();
			}

			// Récupérer tous les noms dans 'comboBoxPieceName' et charger le dernier élément
			{
				int cpt = 0;
				for each (Generator^ generator in generators) {
					comboBoxPieceName->Items->Add(++cpt + ". " + generator->name());
				}
				int count = comboBoxPieceName->Items->Count;
				if (count > 0) comboBoxPieceName->SelectedIndex = count - 1;
			}

			// Sélectionner le bouton 'Générer'
			buttonGenerate->Select();

		}
	protected:
		~MainForm() {

			if (components) delete components;

		}

	// Champs
	private:
		array<Generator^, 1>^ generators;
	
	// Evénements
	private:
		void MainForm_Load(Object^, EventArgs^) {

			comboBoxPieceName->Select();

		}
		void comboBoxPieceName_SelectedIndexChanged(Object^, EventArgs^) {
		
			Generator^ piece = getSelectedGenerator();

			if (((Object^)piece) != nullptr) labelDescription->Text = piece->description();

		}
		void buttonBrowse_Click(Object^, EventArgs^) {
			
			String^ startPath = textBoxSaveDirectory->Text;
			try {
				// Créer le dossier si besoin
				try {
					String^ dir = startPath;
					if (!IO::Directory::Exists(dir)) IO::Directory::CreateDirectory(dir);
				} catch (...) { }
				// Charger le dossier si possible
				folderBrowserDialogSave->SelectedPath = startPath;
			} catch (...) { }
			if (folderBrowserDialogSave->ShowDialog() == Windows::Forms::DialogResult::OK) {
				textBoxSaveDirectory->Text = folderBrowserDialogSave->SelectedPath;
			}

		}
		void buttonGenerate_Click(Object^, EventArgs^) {
		
			// Créer le morceau
			Generator^ generator = getSelectedGenerator();
			if (((Object^)generator) == nullptr) return;
			auto score_image_comments = generator->generate();
			if (score_image_comments == nullptr) return;
			Score ^ score    = score_image_comments->score   ;
			Bitmap^ image    = score_image_comments->image   ;
			String^ comments = score_image_comments->comments; if (comments == nullptr) comments = "";

			// Convertir en MusicXML
			String^ piece_toMusicXML = (score != nullptr) ? score->convertToMusicXML() : "";

			// Afficher le texte MusicXML, la taille du texte, l'image et le commentaire
			textBoxResult   ->Text  = piece_toMusicXML;
			groupBoxResult  ->Text  = formatDescription(score, piece_toMusicXML);
			pictureBoxResult->Image = (image != nullptr) ? image : createVoidImage();
			textBoxComments ->Text  = comments;

			// Essayer d'enregistrer le fichier
			{
				try {
					if (piece_toMusicXML != "" || comments != "") {
						// Créer le dossier s'il n'existe pas encore
						String^ dir = textBoxSaveDirectory->Text;
						if (!IO::Directory::Exists(dir)) IO::Directory::CreateDirectory(dir);
						// Créer le nom du fichier
						String^ name; {
							DateTime now = DateTime::Now;
							String^ name_base = "(" + (now.Year % 100) + "-" + now.Month + "-" + now.Day + " " + now.Hour + "-" + now.Minute + "-" + now.Second + "-" + now.Millisecond + ") " + makeCorrectFileName(generator->name());
							name = name_base + ".xml";
							int cpt = 1;
							while (IO::File::Exists(dir + "\\" + name)) {
								name = name_base + " " + cpt++ + ".xml";
							}
						}
						// Enregistrer le fichier et l'image
						if (piece_toMusicXML != "") IO::File::WriteAllText(dir + "\\" + name, piece_toMusicXML, System::Text::Encoding::UTF8);
						if (image != nullptr) image->Save(dir + "\\" + name->Substring(0, name->Length - 4) + ".bmp");
					}
				} catch (Exception^ e) {
					MessageBox::Show("Erreur lors de l'enregistrement du fichier :\r\n'" + e->Message + "'.");
				}
			}

		}

	// Fonctions
	private:
		Generator^ getSelectedGenerator() {
			
			int index = comboBoxPieceName->SelectedIndex;

			if (index >= 0) return generators[index];
			else            return nullptr;

		}
		static String^ makeCorrectFileName(String^ s) {
			
			List<wchar_t>^ correctChars = gcnew List<wchar_t>;

			for each (wchar_t c in s) {
				if (
					(c >= L'a' && c <= L'z') ||
					(c >= L'A' && c <= L'Z') ||
					(c >= L'0' && c <= L'9') ||
					c == L' ' || c == L'-' || c == L',' || c == L'(' || c == L')' || c == L'{' || c == L'}' || c == L'[' || c == L']' ||
					c == L'@' || c == L'.' || c == L'$' || c == L'%' || c == L'=' || c == L'+' || c == L'!' || c == L';' || c == L'\'' ||
					c == L'~' || c == L'_' || c == L'^' || c == L'¨'
				) {
					correctChars->Add(c);
				}
				else if (c == L'*' || c == L'\\' || c == L'/' || c == L':' || c == L'?' || c == L'<' || c == L'>' || c == L'"' || c == L'|' || c == L'–') correctChars->Add(L'-');
				else if (c == L'à' || c == L'â' || c == L'ä' || c == L'æ') correctChars->Add(L'a');
				else if (c == L'ç'                                       ) correctChars->Add(L'c');
				else if (c == L'é' || c == L'è' || c == L'ê' || c == L'ë') correctChars->Add(L'e');
				else if (c == L'î' || c == L'ï'                          ) correctChars->Add(L'i');
				else if (c == L'ñ'                                       ) correctChars->Add(L'n');
				else if (c == L'ô' || c == L'ö' || c == L'œ'             ) correctChars->Add(L'o');
				else if (c == L'ù' || c == L'û'                          ) correctChars->Add(L'u');
				else if (c == L'ÿ'                                       ) correctChars->Add(L'y');
				else if (c == L'À' || c == L'Â' || c == L'Ä' || c == L'Æ') correctChars->Add(L'A');
				else if (c == L'Ç'                                       ) correctChars->Add(L'C');
				else if (c == L'É' || c == L'È' || c == L'Ê' || c == L'Ë') correctChars->Add(L'E');
				else if (c == L'Î' || c == L'Ï'                          ) correctChars->Add(L'I');
				else if (c == L'Ñ'                                       ) correctChars->Add(L'N');
				else if (c == L'Ô' || c == L'Ö' || c == L'Œ'             ) correctChars->Add(L'O');
				else if (c == L'Ù' || c == L'Û'                          ) correctChars->Add(L'U');
				else if (c == L'Ÿ'                                       ) correctChars->Add(L'Y');
				else                                                       correctChars->Add(L'?');
			}

			return gcnew String(correctChars->ToArray());

		}
		static Bitmap^ createVoidImage() {
			
			int size = 55;
			Bitmap^ ans = gcnew Bitmap(size, size);
			
			Color c1 = Color::FromArgb(255, 192, 127);
			Color c2 = Color::White;

			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					ans->SetPixel(i, j, (i + j) % 2 == 0 ? c1 : c2);
				}
			}

			return ans;

		}
		static String^ formatDescription(Score^ score, String^ pieceAsMusicXML) {
			
			if (score == nullptr) {
				return "Résultat vide";
			} else {
				double xmlLength = 0.001 * pieceAsMusicXML->Length;
				int roundKilos = (xmlLength >= 100) ? 0 : 1;
				double kilos_val = Math::Round(xmlLength, roundKilos);
				String^ parts = score->nbParts + " partie"        + (score->nbParts >= 2 ? "s" : "");
				String^ bars  = score->nbBars  + " mesure"        + (score->nbBars  >= 2 ? "s" : "");
				String^ kilos = kilos_val      + " kilocaractère" + (kilos_val      >= 2 ? "s" : "");
				return "Résultat : " + parts + ", " + bars + ", " + kilos;
			}

		}

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialogSave;
		private: System::Windows::Forms::GroupBox^  groupBoxResult;
		private: System::Windows::Forms::TextBox^  textBoxResult;
		private: System::Windows::Forms::TextBox^  textBoxComments;
		private: System::Windows::Forms::GroupBox^  groupBoxFile;
		private: System::Windows::Forms::TextBox^  textBoxSaveDirectory;
		private: System::Windows::Forms::Button^  buttonBrowse;
		private: System::Windows::Forms::GroupBox^  groupBoxMusicalPiece;
		private: System::Windows::Forms::ComboBox^  comboBoxPieceName;
		private: System::Windows::Forms::Label^  labelDescription;
		private: System::Windows::Forms::Button^  buttonGenerate;
		private: System::Windows::Forms::PictureBox^  pictureBoxResult;
		private:
			System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->groupBoxFile = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxSaveDirectory = (gcnew System::Windows::Forms::TextBox());
			this->buttonBrowse = (gcnew System::Windows::Forms::Button());
			this->groupBoxMusicalPiece = (gcnew System::Windows::Forms::GroupBox());
			this->labelDescription = (gcnew System::Windows::Forms::Label());
			this->comboBoxPieceName = (gcnew System::Windows::Forms::ComboBox());
			this->buttonGenerate = (gcnew System::Windows::Forms::Button());
			this->groupBoxResult = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxComments = (gcnew System::Windows::Forms::TextBox());
			this->pictureBoxResult = (gcnew System::Windows::Forms::PictureBox());
			this->textBoxResult = (gcnew System::Windows::Forms::TextBox());
			this->folderBrowserDialogSave = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->groupBoxFile->SuspendLayout();
			this->groupBoxMusicalPiece->SuspendLayout();
			this->groupBoxResult->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxResult))->BeginInit();
			this->SuspendLayout();
			// 
			// groupBoxFile
			// 
			this->groupBoxFile->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxFile->Controls->Add(this->textBoxSaveDirectory);
			this->groupBoxFile->Controls->Add(this->buttonBrowse);
			this->groupBoxFile->Location = System::Drawing::Point(12, 123);
			this->groupBoxFile->Name = L"groupBoxFile";
			this->groupBoxFile->Size = System::Drawing::Size(622, 55);
			this->groupBoxFile->TabIndex = 1;
			this->groupBoxFile->TabStop = false;
			this->groupBoxFile->Text = L"Dossier de destination";
			// 
			// textBoxSaveDirectory
			// 
			this->textBoxSaveDirectory->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->textBoxSaveDirectory->BackColor = System::Drawing::Color::White;
			this->textBoxSaveDirectory->Location = System::Drawing::Point(6, 22);
			this->textBoxSaveDirectory->Name = L"textBoxSaveDirectory";
			this->textBoxSaveDirectory->ReadOnly = true;
			this->textBoxSaveDirectory->Size = System::Drawing::Size(574, 22);
			this->textBoxSaveDirectory->TabIndex = 0;
			this->textBoxSaveDirectory->Text = L"C:\\ScoreGen generated scores";
			// 
			// buttonBrowse
			// 
			this->buttonBrowse->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->buttonBrowse->Location = System::Drawing::Point(586, 18);
			this->buttonBrowse->Name = L"buttonBrowse";
			this->buttonBrowse->Size = System::Drawing::Size(30, 30);
			this->buttonBrowse->TabIndex = 1;
			this->buttonBrowse->Text = L"...";
			this->buttonBrowse->UseVisualStyleBackColor = true;
			this->buttonBrowse->Click += gcnew System::EventHandler(this, &MainForm::buttonBrowse_Click);
			// 
			// groupBoxMusicalPiece
			// 
			this->groupBoxMusicalPiece->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxMusicalPiece->Controls->Add(this->labelDescription);
			this->groupBoxMusicalPiece->Controls->Add(this->comboBoxPieceName);
			this->groupBoxMusicalPiece->Location = System::Drawing::Point(12, 12);
			this->groupBoxMusicalPiece->Name = L"groupBoxMusicalPiece";
			this->groupBoxMusicalPiece->Size = System::Drawing::Size(622, 105);
			this->groupBoxMusicalPiece->TabIndex = 0;
			this->groupBoxMusicalPiece->TabStop = false;
			this->groupBoxMusicalPiece->Text = L"Morceau à générer";
			// 
			// labelDescription
			// 
			this->labelDescription->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->labelDescription->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(75)), static_cast<System::Int32>(static_cast<System::Byte>(75)), 
				static_cast<System::Int32>(static_cast<System::Byte>(75)));
			this->labelDescription->Location = System::Drawing::Point(6, 48);
			this->labelDescription->Margin = System::Windows::Forms::Padding(3, 0, 3, 3);
			this->labelDescription->Name = L"labelDescription";
			this->labelDescription->Size = System::Drawing::Size(610, 51);
			this->labelDescription->TabIndex = 1;
			this->labelDescription->Text = L"(description)";
			this->labelDescription->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// comboBoxPieceName
			// 
			this->comboBoxPieceName->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBoxPieceName->BackColor = System::Drawing::Color::White;
			this->comboBoxPieceName->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBoxPieceName->FormattingEnabled = true;
			this->comboBoxPieceName->Location = System::Drawing::Point(6, 21);
			this->comboBoxPieceName->Name = L"comboBoxPieceName";
			this->comboBoxPieceName->Size = System::Drawing::Size(610, 24);
			this->comboBoxPieceName->TabIndex = 0;
			this->comboBoxPieceName->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::comboBoxPieceName_SelectedIndexChanged);
			// 
			// buttonGenerate
			// 
			this->buttonGenerate->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->buttonGenerate->Location = System::Drawing::Point(267, 184);
			this->buttonGenerate->Name = L"buttonGenerate";
			this->buttonGenerate->Size = System::Drawing::Size(113, 36);
			this->buttonGenerate->TabIndex = 2;
			this->buttonGenerate->Text = L"Générer";
			this->buttonGenerate->UseVisualStyleBackColor = true;
			this->buttonGenerate->Click += gcnew System::EventHandler(this, &MainForm::buttonGenerate_Click);
			// 
			// groupBoxResult
			// 
			this->groupBoxResult->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxResult->Controls->Add(this->textBoxComments);
			this->groupBoxResult->Controls->Add(this->pictureBoxResult);
			this->groupBoxResult->Controls->Add(this->textBoxResult);
			this->groupBoxResult->Location = System::Drawing::Point(12, 226);
			this->groupBoxResult->Name = L"groupBoxResult";
			this->groupBoxResult->Size = System::Drawing::Size(622, 300);
			this->groupBoxResult->TabIndex = 3;
			this->groupBoxResult->TabStop = false;
			this->groupBoxResult->Text = L"Résultat";
			// 
			// textBoxComments
			// 
			this->textBoxComments->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBoxComments->BackColor = System::Drawing::Color::White;
			this->textBoxComments->Location = System::Drawing::Point(482, 21);
			this->textBoxComments->Multiline = true;
			this->textBoxComments->Name = L"textBoxComments";
			this->textBoxComments->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->textBoxComments->Size = System::Drawing::Size(134, 273);
			this->textBoxComments->TabIndex = 1;
			this->textBoxComments->WordWrap = false;
			// 
			// pictureBoxResult
			// 
			this->pictureBoxResult->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->pictureBoxResult->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(240)), 
				static_cast<System::Int32>(static_cast<System::Byte>(240)), static_cast<System::Int32>(static_cast<System::Byte>(240)));
			this->pictureBoxResult->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pictureBoxResult->Location = System::Drawing::Point(203, 21);
			this->pictureBoxResult->Name = L"pictureBoxResult";
			this->pictureBoxResult->Size = System::Drawing::Size(273, 273);
			this->pictureBoxResult->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxResult->TabIndex = 1;
			this->pictureBoxResult->TabStop = false;
			// 
			// textBoxResult
			// 
			this->textBoxResult->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->textBoxResult->BackColor = System::Drawing::Color::White;
			this->textBoxResult->Location = System::Drawing::Point(6, 21);
			this->textBoxResult->Multiline = true;
			this->textBoxResult->Name = L"textBoxResult";
			this->textBoxResult->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->textBoxResult->Size = System::Drawing::Size(191, 273);
			this->textBoxResult->TabIndex = 0;
			this->textBoxResult->WordWrap = false;
			// 
			// MainForm
			// 
			this->AcceptButton = this->buttonGenerate;
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(646, 538);
			this->Controls->Add(this->groupBoxResult);
			this->Controls->Add(this->buttonGenerate);
			this->Controls->Add(this->groupBoxMusicalPiece);
			this->Controls->Add(this->groupBoxFile);
			this->MinimumSize = System::Drawing::Size(312, 368);
			this->Name = L"MainForm";
			this->ShowIcon = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"ScoreGen";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->groupBoxFile->ResumeLayout(false);
			this->groupBoxFile->PerformLayout();
			this->groupBoxMusicalPiece->ResumeLayout(false);
			this->groupBoxResult->ResumeLayout(false);
			this->groupBoxResult->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxResult))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	};
}
