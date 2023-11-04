#include "dsl.h"

#include <algorithm>
#include <functional>

bool builder::is_image(const std::string &filename)
{
	return endswith(filename, ".png") ||
		   endswith(filename, ".jpg") ||
		   endswith(filename, ".jpeg") ||
		   endswith(filename, ".gif") ||
		   endswith(filename, ".svg") ||
		   endswith(filename, ".bmp") ||
		   endswith(filename, ".tif") ||
		   endswith(filename, ".tiff") ||
		   endswith(filename, ".ico") ||
		   endswith(filename, ".webp") ||
		   endswith(filename, ".avif") ||
		   endswith(filename, ".apng") ||
		   endswith(filename, ".jfif") ||
		   endswith(filename, ".pjpeg") ||
		   endswith(filename, ".pjp") ||
		   endswith(filename, "JPG") ||
		   endswith(filename, "JPEG") ||
		   endswith(filename, "PNG") ||
		   endswith(filename, "GIF") ||
		   endswith(filename, "SVG") ||
		   endswith(filename, "BMP") ||
		   endswith(filename, "TIF") ||
		   endswith(filename, "TIFF") ||
		   endswith(filename, "ICO") ||
		   endswith(filename, "WEBP") ||
		   endswith(filename, "AVIF") ||
		   endswith(filename, "APNG") ||
		   endswith(filename, "JFIF") ||
		   endswith(filename, "PJPEG") ||
		   endswith(filename, "PJP");
}

bool builder::is_audio(const std::string &filename)
{
	return endswith(filename, "mp3") ||
		   endswith(filename, "ogg") ||
		   endswith(filename, "wav") ||
		   endswith(filename, "wave") ||
		   endswith(filename, "MP3") ||
		   endswith(filename, "OGG") ||
		   endswith(filename, "WAV") ||
		   endswith(filename, "WAVE");
}

bool builder::is_video(const std::string &filename)
{
	return endswith(filename, "mp4") ||
		   endswith(filename, "webm") ||
		   endswith(filename, "ogv") ||
		   endswith(filename, "ogg") ||
		   endswith(filename, "MP4") ||
		   endswith(filename, "WEBM") ||
		   endswith(filename, "OGV") ||
		   endswith(filename, "OGG");
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

	html += text;
}

void builder::write_b(const node &n)
{
	html += "<b>";
	write_children(n);
	html += "</b>";
}

void builder::write_i(const node &n)
{
	html += "<i>";
	write_children(n);
	html += "</i>";
}

void builder::write_u(const node &n)
{
	html += "<u>";
	write_children(n);
	html += "</u>";
}

void builder::write_sub(const node &n)
{
	html += "<sub>";
	write_children(n);
	html += "</sub>";
}

void builder::write_sup(const node &n)
{
	html += "<sup>";
	write_children(n);
	html += "</sup>";
}

void builder::write_colour(const node &n)
{
	std::string colour = n.tag_attrs;
	trim(colour);

	if (colour.empty())
	{
		html += "<span style=\"color: darkgreen;\">";
	}
	else
	{
		html += "<span style=\"color: " + colour + ";\">";
	}

	write_children(n);
	html += "</span>";
}

void builder::write_m(const node &n)
{
	html += "<div>";
	write_children(n);
	html += "</div>";
}

void builder::write_m_n(const node &n)
{
	int level = n.tag_name[1] - '0';
	html += "<div style=\"margin-left: " + std::to_string(level * 9) + "px;\">";
	write_children(n);
	html += "</div>";
}

void builder::write_example(const node &n)
{
	html += "<span style=\"color: grey;\">";
	write_children(n);
	html += "</span>";
}

void builder::write_media(const node &n)
{
	std::string filename = n.to_string();
	trim(filename);
	resources_name.push_back(filename);

	if (is_image(filename))
	{
		html += "<img src=\"" + url_cache_root + filename + "\" alt=\"" + filename + "\"/>";
	}
	else if (is_audio(filename))
	{
		if (audio_found)
		{
			html += "<audio controls src=\"" + url_cache_root + filename + "\">" + filename + "</audio>";
		}
		else
		{
			html += "<audio controls autoplay src=\"" + url_cache_root + filename + "\">" + filename + "</audio>";
			audio_found = true;
		}
	}
	else if (is_video(filename))
	{
		html += "<video controls src=\"" + url_cache_root + filename + "\">" + filename + "</video>";
	}
	else
	{
		html += "<a href=\"" + url_cache_root + filename + "\">" + filename + "</a>";
	}
}

void builder::write_ref(const node &n)
{
	std::string headword = get_node_link(n);
	html += "<a href=\"" + url_lookup_root + headword + "\">" + headword + "</a>";
}

void builder::write_url(const node &n)
{
	std::string url = get_node_link(n);
	html += "<a href=\"" + url + "\">" + url + "</a>";
}

void builder::write_p(const node &n)
{
	html += "<i><font color=\"green\">"; // See rule for dsl_p in GoldenDict's source code
	write_children(n);
	html += "</font></i>";
}

void builder::write_br(const node &n)
{
	html += "<br/>";
	// It won't hurt if we write children here
	write_children(n);
}

void builder::write_unknown(const node &n)
{
	html += "<span>";
	write_children(n);
	html += "</span>";
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
												 audio_found(false), html(std::string())
{
}

void builder::build(const node &root)
{
	write_children(root);
}
