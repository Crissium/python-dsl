#include "dsl.h"

#include <algorithm>
#include <functional>

bool builder::is_image(const std::string &filename)
{
	if (filename.size() > 4)
	{
		std::string end_length_4 = filename.substr(filename.size() - 4);
		if (end_length_4 == ".png" || end_length_4 == ".jpg" || end_length_4 == ".gif" || end_length_4 == ".svg" || end_length_4 == ".bmp" || end_length_4 == ".tif" || end_length_4 == ".ico" || end_length_4 == ".pjp" || end_length_4 == ".PNG" || end_length_4 == ".JPG" || end_length_4 == ".GIF" || end_length_4 == ".SVG" || end_length_4 == ".BMP" || end_length_4 == ".TIF" || end_length_4 == ".ICO" || end_length_4 == ".PJP")
		{
			return true;
		}
	}

	if (filename.size() > 5)
	{
		std::string end_length_5 = filename.substr(filename.size() - 5);
		if (end_length_5 == ".jpeg" || end_length_5 == ".tiff" || end_length_5 == ".webp" || end_length_5 == ".avif" || end_length_5 == ".apng" || end_length_5 == ".jfif" || end_length_5 == ".JPEG" || end_length_5 == ".TIFF" || end_length_5 == ".WEBP" || end_length_5 == ".AVIF" || end_length_5 == ".APNG" || end_length_5 == ".JFIF")
		{
			return true;
		}
	}

	if (filename.size() > 6)
	{
		std::string end_length_6 = filename.substr(filename.size() - 6);
		if (end_length_6 == ".pjpeg" || end_length_6 == ".PJPEG")
		{
			return true;
		}
	}

	return false;
}

bool builder::is_audio(const std::string &filename)
{
	if (filename.size() > 4)
	{
		std::string end_length_4 = filename.substr(filename.size() - 4);
		if (end_length_4 == ".mp3" || end_length_4 == ".ogg" || end_length_4 == ".wav" || end_length_4 == ".MP3" || end_length_4 == ".OGG" || end_length_4 == ".WAV")
		{
			return true;
		}
	}

	if (filename.size() > 5)
	{
		std::string end_length_5 = filename.substr(filename.size() - 5);
		if (end_length_5 == ".wave" || end_length_5 == ".WAVE")
		{
			return true;
		}
	}

	return false;
}

bool builder::is_video(const std::string &filename)
{
	if (filename.size() > 4)
	{
		std::string end_length_4 = filename.substr(filename.size() - 4);
		if (end_length_4 == ".mp4" || end_length_4 == ".ogv" || end_length_4 == ".ogg" || end_length_4 == ".MP4" || end_length_4 == ".OGV" || end_length_4 == ".OGG")
		{
			return true;
		}
	}

	if (filename.size() > 5)
	{
		std::string end_length_5 = filename.substr(filename.size() - 5);
		if (end_length_5 == ".webm" || end_length_5 == ".WEBM")
		{
			return true;
		}
	}

	return false;
}

std::string builder::get_node_link(const node &n)
{
	std::string link_text;

	if (!n.tag_attrs.empty())
	{
		std::size_t i = n.tag_attrs.find("target=\"");
		if (i > 0)
		{
			std::size_t end = n.tag_attrs.find("\"", i + 8);
			if (end > i + 8)
			{
				link_text = n.tag_attrs.substr(i + 8, end - i - 8);
			}
			else
			{
				link_text = n.tag_attrs.substr(i + 8);
			}
		}
	}

	if (link_text.empty())
	{
		link_text = n.to_string();
	}

	trim(link_text);
	return html_escape(link_text);
}

void builder::write_children(const node &n)
{
	std::for_each(n.cbegin(), n.cend(), std::bind(&builder::node_to_html, this, std::placeholders::_1));
}

void builder::write_text(const node &n)
{
	std::string text = html_escape(n.text);

	// Handle all end-of-line
	std::size_t i;

	// Strip '\r'
	while ((i = text.find('\r')) != std::string::npos)
	{
		text.erase(i, 1);
	}

	// Replace all '\n'
	while ((i = text.find('\n')) != std::string::npos)
	{
		// text.replace(i, 1, "<br />");
		text.erase(i, 1);
	}

	html_stream << text;
}

void builder::write_b(const node &n)
{
	html_stream << "<b>";
	write_children(n);
	html_stream << "</b>";
}

void builder::write_i(const node &n)
{
	html_stream << "<i>";
	write_children(n);
	html_stream << "</i>";
}

void builder::write_u(const node &n)
{
	html_stream << "<u>";
	write_children(n);
	html_stream << "</u>";
}

void builder::write_sub(const node &n)
{
	html_stream << "<sub>";
	write_children(n);
	html_stream << "</sub>";
}

void builder::write_sup(const node &n)
{
	html_stream << "<sup>";
	write_children(n);
	html_stream << "</sup>";
}

void builder::write_colour(const node &n)
{
	std::string colour = n.tag_attrs;
	trim(colour);

	if (colour.empty())
	{
		html_stream << "<span style=\"color: darkgreen;\">";
	}
	else
	{
		html_stream << "<span style=\"color: " << colour << ";\">";
	}

	write_children(n);
	html_stream << "</span>";
}

void builder::write_m(const node &n)
{
	html_stream << "<div>";
	write_children(n);
	html_stream << "</div>";
}

void builder::write_m_n(const node &n)
{
	int level = n.tag_name[1] - '0';
	html_stream << "<div style=\"margin-left: " << std::to_string(level * 9) << "px;\">";
	write_children(n);
	html_stream << "</div>";
}

void builder::write_example(const node &n)
{
	html_stream << "<span style=\"color: grey;\">";
	write_children(n);
	html_stream << "</span>";
}

void builder::write_media(const node &n)
{
	std::string filename = n.to_string();
	trim(filename);
	resources_name.push_back(filename);

	if (is_image(filename))
	{
		html_stream << "<img src=\"" << url_cache_root << filename << "\" alt=\"" << filename << "\"/>";
	}
	else if (is_audio(filename))
	{
		if (audio_found)
		{
			html_stream << "<audio controls src=\"" << url_cache_root << filename << "\">" << filename << "</audio>";
		}
		else
		{
			html_stream << "<audio controls autoplay src=\"" << url_cache_root << filename << "\">" << filename << "</audio>";
			audio_found = true;
		}
	}
	else if (is_video(filename))
	{
		html_stream << "<video controls src=\"" << url_cache_root << filename << "\">" << filename << "</video>";
	}
	else
	{
		html_stream << "<a href=\"" << url_cache_root << filename << "\">" << filename << "</a>";
	}
}

void builder::write_ref(const node &n)
{
	std::string headword = get_node_link(n);
	html_stream << "<a href=\"" << url_lookup_root << headword << "\">" << headword << "</a>";
}

void builder::write_url(const node &n)
{
	std::string url = get_node_link(n);
	html_stream << "<a href=\"" << url << "\">" << url << "</a>";
}

void builder::write_p(const node &n)
{
	// See rule for dsl_p in GoldenDict's source code
	html_stream << "<span style=\"color: green; font-style: italic;\">";
	write_children(n);
	html_stream << "</span>";
}

void builder::write_br(const node &n)
{
	html_stream << "<br/>";
	// It won't hurt if we write children here
	write_children(n);
}

void builder::write_unknown(const node &n)
{
	html_stream << "<span>";
	write_children(n);
	html_stream << "</span>";
}

void builder::node_to_html(const node &n)
{
	if (!n.is_tag)
	{
		write_text(n);
	}
	else
	{
		if (n.tag_name == "b")
		{
			write_b(n);
		}
		else if (n.tag_name == "i")
		{
			write_i(n);
		}
		else if (n.tag_name == "u" || n.tag_name == "'")
		{
			write_u(n);
		}
		else if (n.tag_name == "sub")
		{
			write_sub(n);
		}
		else if (n.tag_name == "sup")
		{
			write_sup(n);
		}
		else if (n.tag_name == "c")
		{
			write_colour(n);
		}
		else if (n.tag_name == "m")
		{
			write_m(n);
		}
		else if (n.tag_name.size() == 2 && n.tag_name[0] == 'm' && std::isdigit(n.tag_name[1]))
		{
			write_m_n(n);
		}
		else if (n.tag_name == "ex")
		{
			write_example(n);
		}
		else if (n.tag_name == "s" || n.tag_name == "video")
		{
			write_media(n);
		}
		else if (n.tag_name == "ref")
		{
			write_ref(n);
		}
		else if (n.tag_name == "url")
		{
			write_url(n);
		}
		else if (n.tag_name == "p")
		{
			write_p(n);
		}
		else if (n.tag_name == "br")
		{
			write_br(n);
		}
		else
		{
			write_unknown(n);
		}
	}
}

builder::builder(const std::string &name_dict) : url_cache_root("/api/cache/" + name_dict + "/"),
												 url_lookup_root("/api/lookup/" + name_dict + "/"),
												 audio_found(false)
{
}

std::string builder::get_html(const node &root)
{
	write_children(root);
	return html_stream.str();
}
