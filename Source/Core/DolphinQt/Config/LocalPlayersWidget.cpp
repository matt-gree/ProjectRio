// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <iosfwd>
#include "DolphinQt/Config/LocalPlayersWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QScrollBar>

#include <utility>
#include <vector>

#include "Common/CommonPaths.h"

#include "Common/FileSearch.h"
#include "Common/FileUtil.h"
#include "Common/IniFile.h"

#include "Core/LocalPlayersConfig.h"
#include "DolphinQt/Config/AddLocalPlayers.h"

#include "Core/ConfigManager.h"
#include "Core/Core.h"
#include "Core/HW/SI/SI.h"
#include "Core/HW/SI/SI_Device.h"

#include "Common/TagSet.h"

#include "DolphinQt/Settings.h"
#include "DolphinQt/QtUtils/ModalMessageBox.h"

std::map<int, std::optional<Tag::TagSet>> m_tagset_combobox_map;  // maps combobox index to tagset

LocalPlayersWidget::LocalPlayersWidget(QWidget* parent) : QWidget(parent)
{
  LocalPlayers::LoadLocalPorts();
  CreateLayout();
  UpdatePlayers();
  PopulateTagsetCombobox();
  ConnectWidgets();
  SetTagSet();

  // Reflect persisted Rio-codes toggle and current emulation state.
  {
    const QSignalBlocker blocker(m_enable_rio_codes);
    m_enable_rio_codes->setChecked(LoadRioCodesEnabled());
  }
  ApplyLockState();

  // React to emulation state changes so inputs lock down without needing
  // the window to be reopened.
  connect(&Settings::Instance(), &Settings::EmulationStateChanged, this,
          [this](Core::State) { ApplyLockState(); });
}

// create the basic UI elements for the local players widget
void LocalPlayersWidget::CreateLayout()
{
  m_player_box = new QGroupBox(tr("Rio Accounts"));
  m_options_box = new QGroupBox(tr("Options"));
  m_player_layout = new QGridLayout();
  m_player_list = new QListWidget;
  m_local_tagset = new QComboBox();

  const auto line_height = QFontMetrics(font()).lineSpacing();
  m_game_mode_description = new QTextEdit;
  m_game_mode_description->setReadOnly(true);
  m_game_mode_description->setFixedHeight(line_height * 10);
  m_game_mode_description->setVerticalScrollBarPolicy(
      Qt::ScrollBarAlwaysOn);

  m_enable_rio_codes = new QCheckBox(tr("Enable Project Rio code injection (disables memory cards)"));
  m_enable_rio_codes->setToolTip(
      tr("Loads Project Rio's built-in Gecko codes and uses the expanded Gecko "
         "code region. Required to use Game Modes and for the stat tracker, HUD, "
         "and other Rio features. Disables memory-card support in local play.\n\n"
         "Mirrors the toggle in the per-game Gecko Codes tab. Read at boot, so "
         "it cannot be changed while emulation is running."));

  m_rio_codes_hint = new QLabel(tr(
      "Enable Project Rio code injection above to use Game Modes."));
  m_rio_codes_hint->setWordWrap(true);
  m_rio_codes_hint->setStyleSheet(QStringLiteral("color: gray; font-style: italic;"));

  m_tagset_label = new QLabel(tr("Game Mode:"));

  auto* tagset_description = new QLabel;
  tagset_description->setText(tr(
    "Game Modes are pre-made ways to play the game.<br/>"
    "Any necessary mods and/or game changes<br/>"
    "are automatically applied when selecting<br/>"
    "a Game Mode.<br/><br/>"
    "Head to the <a href=\"https://www.projectrio.online/gamemode/\">Project Rio Website</a><br/>"
    "to learn more about Game Modes!"
  ));
  tagset_description->setTextFormat(Qt::RichText);
  tagset_description->setTextInteractionFlags(Qt::TextBrowserInteraction);
  tagset_description->setOpenExternalLinks(true);

  auto* gc_label0 = new QLabel(tr("Online Account:"));
  auto* gc_box0 = m_player_list_0 = new QComboBox();

  auto* gc_label1 = new QLabel(tr("Player 1:"));
  auto* gc_box1 = m_player_list_1 = new QComboBox();

  auto* gc_label2 = new QLabel(tr("Player 2:"));
  auto gc_box2 = m_player_list_2 = new QComboBox();

  auto* gc_label3 = new QLabel(tr("Player 3:"));
  auto gc_box3 = m_player_list_3 = new QComboBox();
  
  auto* gc_label4 = new QLabel(tr("Player 4:"));
  auto gc_box4 = m_player_list_4 = new QComboBox();

  m_port_array = {m_player_list_0, m_player_list_1, m_player_list_2, m_player_list_3,
                  m_player_list_4};

  m_add_button = new QPushButton(tr("Add Player"));
  m_add_button->setToolTip(
      (tr("Local Players System:\n\nAdd players using the \"Add Player\" button.\n"
          "The Local Players are used for recording stats locally.\nThese players are not used "
          "for online games.\n"
          "It is only used for keeping track of stats for offline games.\n\n"
          "Make sure to obtain the EXACT Username and Key of the player you wish to add.\n"
          "The Local Players can be changed while a game session is running.")));

  m_remove_button = new QPushButton(tr("Remove Player"));

  m_player_layout->addWidget(gc_label0, 0, 0);
  m_player_layout->addWidget(gc_box0, 0, 1, Qt::AlignLeft);
  m_player_layout->addWidget(new QLabel(tr(" ")), 1, 0);
  m_player_layout->addWidget(gc_label1, 2, 0);
  m_player_layout->addWidget(gc_box1, 2, 1, Qt::AlignLeft);
  m_player_layout->addWidget(gc_label2, 3, 0);
  m_player_layout->addWidget(gc_box2, 3, 1, Qt::AlignLeft);
  m_player_layout->addWidget(gc_label3, 4, 0);
  m_player_layout->addWidget(gc_box3, 4, 1, Qt::AlignLeft);
  m_player_layout->addWidget(gc_label4, 5, 0);
  m_player_layout->addWidget(gc_box4, 5, 1, Qt::AlignLeft);

  auto* player_layout = new QVBoxLayout;
  player_layout->addLayout(m_player_layout, 1);
  player_layout->addWidget(m_add_button, 1);
  player_layout->addSpacing(20);
  player_layout->addWidget(m_player_list, 10);
  player_layout->addWidget(m_remove_button, 1);
  m_player_box->setLayout(player_layout);

  auto* options_layout = new QGridLayout;
  options_layout->setAlignment(Qt::AlignTop);
  options_layout->addWidget(m_enable_rio_codes, 0, 0, 1, -1);
  options_layout->addWidget(m_rio_codes_hint, 1, 0, 1, -1);
  options_layout->addWidget(m_tagset_label, 2, 0);
  options_layout->addWidget(m_local_tagset, 2, 1, 1, -1, Qt::AlignLeft);
  options_layout->addWidget(tagset_description, 3, 0, 1, -1);
  options_layout->addWidget(m_game_mode_description, 4, 0, 1, -1);
  m_options_box->setLayout(options_layout);

  auto* layout = new QHBoxLayout;
  layout->addWidget(m_player_box, 1);
  layout->addWidget(m_options_box);
  layout->addSpacing(20);

  setLayout(layout);
}

// add each player avaliable in the ini file into the list widget and port combo boxes
void LocalPlayersWidget::UpdatePlayers()
{
  m_player_list->clear();
  for (auto& port : m_port_array)
  {
    port->clear();
  }

  LocalPlayers::LocalPlayers localplayers;
  m_local_players = localplayers.GetPlayers();
  m_player_map = localplayers.GetPlayerMap();
  m_player_index_map = localplayers.GetPlayerIndexMap();

  // List avalable players in LocalPlayers.ini
  for (size_t i = 0; i < m_local_players.size(); i++)
  {
    const auto& player = m_local_players[i];

    auto username = QString::fromStdString(player.username);

    // In the future, i should add in a feature that if a player is selected on another port, they
    // won't appear on the dropdown some conditional that checks the other ports before adding the
    // item
    for (auto& port : m_port_array)
    {
      port->addItem(username);
    }
    if (i > 0)
      m_player_list->addItem(username);
  }

  SetPlayers();
}

// take the player that is set to the player vars and save it to the ini file
// then make sure the combo box is set to the correct position
void LocalPlayersWidget::SetPlayers()
{
  LocalPlayers::SaveLocalPorts();

  m_player_list_0->setCurrentIndex(m_player_index_map[LocalPlayers::m_online_player.userid]);
  m_player_list_1->setCurrentIndex(m_player_index_map[LocalPlayers::m_local_player_1.userid]);
  m_player_list_2->setCurrentIndex(m_player_index_map[LocalPlayers::m_local_player_2.userid]); 
  m_player_list_3->setCurrentIndex(m_player_index_map[LocalPlayers::m_local_player_3.userid]);
  m_player_list_4->setCurrentIndex(m_player_index_map[LocalPlayers::m_local_player_4.userid]);
}

void LocalPlayersWidget::OnRemovePlayers()
{
  if (m_player_list->currentItem() == nullptr)
    return;

  const int index = m_player_list->currentRow() + 1;

  m_local_players.erase(m_local_players.begin() + index);

  for (auto& port : m_port_array)
  {
    if (port->currentIndex() == index)
    {
      port->setCurrentIndex(0);
    }
  }

  SetPortInfo();
  UpdatePlayers();
}

void LocalPlayersWidget::OnAddPlayers()
{
  LocalPlayers::LocalPlayers::Player name;

  AddLocalPlayersEditor ed(this);
  ed.SetPlayer(&name);
  if (ed.exec() == QDialog::Rejected)
    return;

  m_local_players.push_back(std::move(name));
  SavePlayers();
  UpdatePlayers();
}

// take player vector and save it to the ini file
void LocalPlayersWidget::SavePlayers()
{
  const auto ini_path = std::string(File::GetUserPath(F_LOCALPLAYERSCONFIG_IDX));

  Common::IniFile local_players_path;
  local_players_path.Load(ini_path);
  LocalPlayers::SavePlayers(local_players_path, m_local_players);
  local_players_path.Save(ini_path);
}

void LocalPlayersWidget::SetPortInfo()
{
  LocalPlayers::m_online_player.SetUserInfo(m_local_players[m_player_list_0->currentIndex()]);
  LocalPlayers::m_local_player_1.SetUserInfo(m_local_players[m_player_list_1->currentIndex()]);
  LocalPlayers::m_local_player_2.SetUserInfo(m_local_players[m_player_list_2->currentIndex()]);
  LocalPlayers::m_local_player_3.SetUserInfo(m_local_players[m_player_list_3->currentIndex()]);
  LocalPlayers::m_local_player_4.SetUserInfo(m_local_players[m_player_list_4->currentIndex()]);

  LocalPlayers::SaveLocalPorts();
  SavePlayers();
  PopulateTagsetCombobox();
}

void LocalPlayersWidget::PopulateTagsetCombobox()
{
  m_local_tagset->clear();
  m_tagset_combobox_map.clear();

  int combobox_index = 0;
  m_local_tagset->addItem(QString::fromStdString("No Game Mode Selected"));
  m_tagset_combobox_map.insert(std::pair<int, std::optional<Tag::TagSet>>(combobox_index++, std::nullopt));

  std::vector<std::map<int, Tag::TagSet>> valid_tagsets;

  std::string player1 = LocalPlayers::m_local_player_1.username;
  std::string player2 = LocalPlayers::m_local_player_2.username;
  std::string player3 = LocalPlayers::m_local_player_3.username;
  std::string player4 = LocalPlayers::m_local_player_4.username;

  std::string player1key = LocalPlayers::m_local_player_1.userid;
  std::string player2key = LocalPlayers::m_local_player_2.userid;
  std::string player3key = LocalPlayers::m_local_player_3.userid;
  std::string player4key = LocalPlayers::m_local_player_4.userid;

  // Validate each player
  // Player 1
  if ((player1 != "No Player Selected" || player1key != "0") && (player1 != "" || player1key != ""))
  {
    if (!IsValidUser(LocalPlayers::m_local_player_1))
    {
      return;
    }
    valid_tagsets.push_back(Tag::getAvailableTagSets(m_http, player1key));
  }

  // Player 2
  if ((player2 != "No Player Selected" || player2key != "0") && (player2 != "" || player2key != ""))
  {
    if (!IsValidUser(LocalPlayers::m_local_player_2))
    {
      return;
    }
    valid_tagsets.push_back(Tag::getAvailableTagSets(m_http, player2key));
  }

  // Player 3
  if ((player3 != "No Player Selected" || player3key != "0") && (player3 != "" || player3key != ""))
  {
    if (!IsValidUser(LocalPlayers::m_local_player_3))
    {
      return;
    }
    valid_tagsets.push_back(Tag::getAvailableTagSets(m_http, player3key));
  }

  // Player 4
  if ((player4 != "No Player Selected" || player4key != "0") && (player4 != "" || player4key != ""))
  {
    if (!IsValidUser(LocalPlayers::m_local_player_4))
    {
      return;
    }
    valid_tagsets.push_back(Tag::getAvailableTagSets(m_http, player1key));
  }

  if (valid_tagsets.size() == 0)
    return;

  // find common tagsets between all the players
  for (auto& tagset : valid_tagsets[0])
  {
    bool add_tagset = true;
    for (auto &player_tagsets : valid_tagsets)
    {
      if (player_tagsets.find(tagset.first) == player_tagsets.end())
      {
        add_tagset = false;
      }
    }
    if (add_tagset)
    {
      m_local_tagset->addItem(QString::fromStdString(tagset.second.name));
      m_tagset_combobox_map.insert(std::pair<int, Tag::TagSet>(combobox_index++, tagset.second));
    }
  }

  m_local_tagset->setCurrentIndex(0);  // set it to nothing selected
}

void LocalPlayersWidget::SetTagSet()
{
  std::optional<Tag::TagSet> selected_tagset = m_tagset_combobox_map[m_local_tagset->currentIndex()];
  Core::SetTagSet(selected_tagset, false);

  m_game_mode_description->clear();
  if (!selected_tagset.has_value())
  {
    m_game_mode_description->append(tr("No Game Mode Selected."));
    return;
  }

  std::vector<std::string> tags = selected_tagset.value().tag_names_vector();
  std::string tags_string = "\nRules:\n";
  for (auto& tag : tags)
  {
    if (tag != selected_tagset.value().name)
      tags_string.append("- " + tag + "\n");
  }
  tags_string.pop_back();  // remove final delimiter

  m_game_mode_description->append(QString::fromStdString(selected_tagset.value().description()));
  m_game_mode_description->append(QString::fromStdString(tags_string));

  QScrollBar* scrollBar = m_game_mode_description->verticalScrollBar();
  scrollBar->setValue(scrollBar->minimum());  // set scroll bar to the top
}

bool LocalPlayersWidget::IsValidUser(LocalPlayers::LocalPlayers::Player player)
{
  LocalPlayers::LocalPlayers::AccountValidationType type = player.ValidateAccount(m_http);

  if (type == LocalPlayers::LocalPlayers::Invalid)
  {
    std::string errormsg = "Invalid Rio Account: " + player.username + "\n\nMake sure to use a valid Rio account, or check your intenet connection.";
    ModalMessageBox::critical(this, tr("Error"), QString::fromStdString(errormsg));
    return false;
  }
  else
  {
    return true;
  }
}

void LocalPlayersWidget::ConnectWidgets()
{
  connect(m_player_list_0, qOverload<int>(&QComboBox::activated), this,
          &LocalPlayersWidget::SetPortInfo);
  connect(m_player_list_1, qOverload<int>(&QComboBox::activated), this,
          &LocalPlayersWidget::SetPortInfo);
  connect(m_player_list_2, qOverload<int>(&QComboBox::activated), this,
          &LocalPlayersWidget::SetPortInfo);
  connect(m_player_list_3, qOverload<int>(&QComboBox::activated), this,
          &LocalPlayersWidget::SetPortInfo);
  connect(m_player_list_4, qOverload<int>(&QComboBox::activated), this,
          &LocalPlayersWidget::SetPortInfo);

  connect(m_local_tagset, qOverload<int>(&QComboBox::activated), this,
          &LocalPlayersWidget::SetTagSet);

  connect(m_add_button, &QPushButton::clicked, this, &LocalPlayersWidget::OnAddPlayers);
  connect(m_remove_button, &QPushButton::clicked, this, &LocalPlayersWidget::OnRemovePlayers);

  connect(m_enable_rio_codes, &QCheckBox::toggled, this, &LocalPlayersWidget::OnRioCodesToggled);
}

bool LocalPlayersWidget::LoadRioCodesEnabled() const
{
  // Project Rio is MSSB-centric, so the toggle lives in GYQE01's per-game ini
  // alongside the matching widget in the Gecko Codes tab.
  Common::IniFile game_ini_local;
  game_ini_local.Load(File::GetUserPath(D_GAMESETTINGS_IDX) + std::string("GYQE01.ini"));

  bool enabled = false;
  if (const auto* core_section = game_ini_local.GetSection("Core"))
    core_section->Get("UseExpandedGeckoSpace", &enabled, false);
  return enabled;
}

void LocalPlayersWidget::SaveRioCodesEnabled(bool enabled)
{
  const auto ini_path =
      std::string(File::GetUserPath(D_GAMESETTINGS_IDX)) + std::string("GYQE01.ini");

  Common::IniFile game_ini_local;
  game_ini_local.Load(ini_path);
  game_ini_local.GetOrCreateSection("Core")->Set("UseExpandedGeckoSpace", enabled);
  game_ini_local.Save(ini_path);
}

void LocalPlayersWidget::OnRioCodesToggled(bool enabled)
{
  SaveRioCodesEnabled(enabled);

  // If the user turns the codes off, also clear any selected local Game Mode
  // so an already-selected TagSet does not force codes back on at boot
  // (see Core::getGameFreeMemory and GeckoCodeConfig::LoadCodes).
  if (!enabled && m_local_tagset->currentIndex() != 0)
  {
    m_local_tagset->setCurrentIndex(0);
    SetTagSet();
  }

  ApplyLockState();
}

void LocalPlayersWidget::ApplyLockState()
{
  const bool emulation_running = Core::GetState() != Core::State::Uninitialized;
  const bool rio_codes_enabled = m_enable_rio_codes->isChecked();

  // During emulation: every input is locked because both the Rio-codes toggle
  // and the Game Mode selection are read at boot and have no effect on the
  // running session.
  m_enable_rio_codes->setEnabled(!emulation_running);
  m_add_button->setEnabled(!emulation_running);
  m_remove_button->setEnabled(!emulation_running);
  m_player_list->setEnabled(!emulation_running);
  for (auto* port : m_port_array)
    port->setEnabled(!emulation_running);

  // Game-mode controls are also gated on the Rio-codes toggle: without the
  // built-in codes, Game Modes will not load correctly, so the dropdown is
  // greyed out and a hint is shown.
  const bool tagset_usable = rio_codes_enabled && !emulation_running;
  m_tagset_label->setEnabled(tagset_usable);
  m_local_tagset->setEnabled(tagset_usable);
  m_game_mode_description->setEnabled(tagset_usable);
  m_rio_codes_hint->setVisible(!rio_codes_enabled);
}
