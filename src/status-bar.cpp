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
#include <selain/status-bar.hpp>
#include <selain/theme.hpp>
#include <selain/utils.hpp>

namespace selain
{
  StatusBar::StatusBar()
    : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL)
    , m_mode_label("NORMAL")
  {
    const auto& font = utils::get_monospace_font();

    set_border_width(0);

    pack_start(m_mode_label, Gtk::PACK_SHRINK);
    pack_start(m_status_label, true, true);

    override_background_color(theme::status_bar_background);

    m_mode_label.override_font(font);
    m_mode_label.override_background_color(theme::mode_bar_normal_background);
    m_mode_label.override_color(theme::mode_bar_normal_foreground);
    m_mode_label.set_halign(Gtk::ALIGN_CENTER);
    m_mode_label.set_justify(Gtk::JUSTIFY_CENTER);

    m_status_label.override_font(font);
    m_status_label.override_background_color(theme::status_bar_background);
    m_status_label.override_color(theme::status_bar_foreground);
    m_status_label.set_halign(Gtk::ALIGN_START);
    m_status_label.set_justify(Gtk::JUSTIFY_LEFT);

    show_all_children();
  }

  void
  StatusBar::set_mode(Mode mode)
  {
    m_mode_label.set_text(get_mode_text(mode));
    if (mode == Mode::INSERT)
    {
      m_mode_label.override_background_color(theme::mode_bar_insert_background);
      m_mode_label.override_color(theme::mode_bar_insert_foreground);
    } else {
      m_mode_label.override_background_color(theme::mode_bar_normal_background);
      m_mode_label.override_color(theme::mode_bar_normal_foreground);
    }
  }

  void
  StatusBar::set_status(const Glib::ustring& status)
  {
    m_status_label.set_text(status.empty() ? m_permanent_status : status);
  }

  void
  StatusBar::set_permanent_status(const Glib::ustring& status)
  {
    m_permanent_status = status;
    m_status_label.set_text(status);
  }

  void
  StatusBar::add_notification(const Glib::ustring& status,
                              NotificationType type,
                              int timeout)
  {
    Gdk::RGBA background;
    Gdk::RGBA foreground;

    switch (type)
    {
      case NotificationType::ERROR:
        background = theme::status_bar_error_background;
        foreground = theme::status_bar_error_foreground;
        break;

      default:
        background = theme::status_bar_background;
        foreground = theme::status_bar_foreground;
        break;
    }

    m_status_label.override_background_color(background);
    m_status_label.override_color(foreground);
    m_status_label.set_text(status);

    Glib::signal_timeout().connect_once(
      sigc::mem_fun(*this, &StatusBar::on_notification_reset),
      timeout * 1000
    );
  }

  void
  StatusBar::on_notification_reset()
  {
    m_status_label.override_background_color(theme::status_bar_background);
    m_status_label.override_color(theme::status_bar_foreground);
    set_status(Glib::ustring());
  }
}
