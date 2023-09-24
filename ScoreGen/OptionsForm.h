#pragma region head

	#pragma once

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

#pragma endregion

namespace ScoreGen {
	public ref class OptionsForm : public System::Windows::Forms::Form {

	// Constructeur, destructeur, appel
	protected:
		OptionsForm() :
			validatedAndClosed(false)
		{

			// Initialisation
			InitializeComponent();

			// Premiers affichages
			Text = "";
			labelDescription->Text = "";

		}
		OptionsForm(String^ title, String^ description) :
			validatedAndClosed(false)
		{

			// Initialisation
			InitializeComponent();

			// Premiers affichages
			Text = title + " – Paramètres";
			labelDescription->Text = description;

		}
	protected:
		~OptionsForm() {

			if (components) delete components;

		}

	// Evénements
	private:
		void OptionsForm_Resize(Object^, EventArgs^) {
			
			labelDescription->Width = Width - 30;
			buttonOk    ->Location = Point(Width / 2 - 116, Height - 74);
			buttonCancel->Location = Point(Width / 2      , Height - 74);

		}
		void buttonOk_Click(Object^, EventArgs^) {
		
			validatedAndClosed = true;
			Close();

		}
		void buttonCancel_Click(Object^, EventArgs^) {
		
			Close();

		}

	// Accès
	public:
		array<Object^, 1>^ getOptions() {
		
			if (validatedAndClosed) return getOptionsInFormValues();
			else                    return nullptr;
		
		}
	protected:
		void setOkButtonEnabled(bool enabled) {
			
			buttonOk->Enabled = enabled;

		}

	// A implémenter
	protected:
		bool validatedAndClosed;
	protected:
		virtual array<Object^, 1>^ getOptionsInFormValues() { return nullptr; }

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::Label^  labelDescription;
		private: System::Windows::Forms::Button^  buttonOk;
		private: System::Windows::Forms::Button^  buttonCancel;
		private:
			System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->labelDescription = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// buttonOk
			// 
			this->buttonOk->Location = System::Drawing::Point(134, 426);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(110, 32);
			this->buttonOk->TabIndex = 0;
			this->buttonOk->Text = L"Valider";
			this->buttonOk->UseVisualStyleBackColor = true;
			this->buttonOk->Click += gcnew System::EventHandler(this, &OptionsForm::buttonOk_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(250, 426);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(110, 32);
			this->buttonCancel->TabIndex = 1;
			this->buttonCancel->Text = L"Annuler";
			this->buttonCancel->UseVisualStyleBackColor = true;
			this->buttonCancel->Click += gcnew System::EventHandler(this, &OptionsForm::buttonCancel_Click);
			// 
			// labelDescription
			// 
			this->labelDescription->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(240)), 
				static_cast<System::Int32>(static_cast<System::Byte>(240)), static_cast<System::Int32>(static_cast<System::Byte>(240)));
			this->labelDescription->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->labelDescription->Location = System::Drawing::Point(12, 9);
			this->labelDescription->Name = L"labelDescription";
			this->labelDescription->Size = System::Drawing::Size(470, 72);
			this->labelDescription->TabIndex = 2;
			this->labelDescription->Text = L"(Description)";
			// 
			// OptionsForm
			// 
			this->AcceptButton = this->buttonOk;
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->buttonCancel;
			this->ClientSize = System::Drawing::Size(494, 470);
			this->Controls->Add(this->labelDescription);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOk);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"OptionsForm";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"(Titre)";
			this->Resize += gcnew System::EventHandler(this, &OptionsForm::OptionsForm_Resize);
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
