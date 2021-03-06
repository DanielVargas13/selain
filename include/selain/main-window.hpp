/*
 * Copyright (c) 2019, Rauli Laine
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef SELAIN_MAIN_WINDOW_HPP_GUARD
#define SELAIN_MAIN_WINDOW_HPP_GUARD

#include <unordered_map>

#include <gtkmm.h>

#include <selain/command.hpp>
#include <selain/command-entry.hpp>
#include <selain/status-bar.hpp>
#include <selain/tab.hpp>

namespace selain
{
  /**
   * Main window for the browser.
   */
  class MainWindow : public Gtk::ApplicationWindow
  {
  public:
    using command_mapping_type = std::unordered_map<std::string, Command>;

    explicit MainWindow(const Glib::RefPtr<Gtk::Application>& application);

    /**
     * Returns the mapping of all known browser commands.
     */
    inline command_mapping_type& get_command_mapping()
    {
      return m_command_mapping;
    }

    /**
     * Returns the mapping of all known browser commands.
     */
    inline const command_mapping_type& get_command_mapping() const
    {
      return m_command_mapping;
    }

    /**
     * Returns the current mode of the window.
     */
    inline Mode get_mode() const
    {
      return m_mode;
    }

    /**
     * Sets the current mode of the window.
     */
    void set_mode(Mode mode);

    /**
     * Returns the status bar of the window.
     */
    inline StatusBar& get_status_bar()
    {
      return m_status_bar;
    }

    /**
     * Returns the status bar of the window.
     */
    inline const StatusBar& get_status_bar() const
    {
      return m_status_bar;
    }

    /**
     * Returns the text boxed where commands are being typed.
     */
    inline CommandEntry& get_command_entry()
    {
      return m_command_entry;
    }

    /**
     * Returns the text boxed where commands are being typed.
     */
    inline const CommandEntry& get_command_entry() const
    {
      return m_command_entry;
    }

    /**
     * Returns pointer to the current tab, or null pointer if no tabs are open.
     */
    Tab* get_current_tab();

    /**
     * Returns pointer to the current tab, or null pointer if no tabs are open.
     */
    const Tab* get_current_tab() const;

    /**
     * Returns pointer to tab from given index, or null pointer if given index
     * is out of bounds.
     */
    Tab* get_nth_tab(int index);

    /**
     * Returns pointer to tab from given index, or null pointer if given index
     * is out of bounds.
     */
    const Tab* get_nth_tab(int index) const;

    Glib::RefPtr<Tab> open_tab(
      const Glib::ustring& uri = Glib::ustring(),
      bool focus = true
    );

    void close_tab(const Tab& tab);
    void close_tab(const Glib::RefPtr<Tab>& tab);

    void set_current_tab(const Glib::RefPtr<Tab>& tab);
    void next_tab();
    void prev_tab();

  private:
    void initialize_commands();

    bool on_command_entry_key_press(::GdkEventKey* event);
    void on_command_received(const Glib::ustring& command);
    void on_tab_status_change(Tab* tab, const Glib::ustring& status);
    void on_tab_switch(Gtk::Widget* widget, ::guint page_number);

  private:
    command_mapping_type m_command_mapping;
    Glib::RefPtr<WebContext> m_web_context;
    Glib::RefPtr<WebSettings> m_web_settings;
    Mode m_mode;
    Gtk::Box m_box;
    Gtk::Notebook m_notebook;
    StatusBar m_status_bar;
    CommandEntry m_command_entry;
  };
}

#endif /* !SELAIN_MAIN_WINDOW_HPP_GUARD */
