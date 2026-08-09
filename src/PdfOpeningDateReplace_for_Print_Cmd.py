#!/usr/bin/env python3
"""Create image-index files beside a group of dropped files.

The command line arguments are treated as the files supplied by a Windows
drag-and-drop operation.  File extensions are deliberately not validated.
"""

from __future__ import annotations

import os
from pathlib import Path
import sys
import tempfile


OUTPUT_ENCODINGS = {
    "FileList.txt": "shift_jis",
    "FileList_outer_frame.txt": "shift_jis",
    "FileList_caption_frame.txt": "shift_jis",
    "index.html.txt": "utf-8",
    "index_outer_frame.html": "shift_jis",
    "index_caption_frame.html": "shift_jis",
    "style_index.css": "shift_jis",
}


def _normalized_parent(path: Path) -> str:
    """Return a Windows-compatible comparison key for a parent directory."""
    return os.path.normcase(os.path.normpath(str(path.parent.resolve())))


def _validate_files(arguments: list[str]) -> tuple[list[Path], Path]:
    if not arguments:
        raise ValueError("ファイルを指定してください。")
    if len(arguments) > 3000:
        raise ValueError("一度に指定できるファイルは3000個までです。")

    files = [Path(argument).expanduser().resolve(strict=True) for argument in arguments]
    if any(not path.is_file() for path in files):
        raise ValueError("通常ファイル以外の項目が含まれています。")

    output_directory = files[0].parent
    expected_parent = _normalized_parent(files[0])
    if any(_normalized_parent(path) != expected_parent for path in files[1:]):
        raise ValueError(
            "異なるフォルダーのファイルが含まれています。"
            "すべてのファイルを同じフォルダーに入れてください。"
        )
    if not os.access(output_directory, os.W_OK):
        raise ValueError(f"出力先フォルダーに書き込めません: {output_directory}")
    return files, output_directory


def _image_tags(files: list[Path]) -> str:
    return "".join(
        f'<img src="./{path.name}" alt="" width="" height="" border="0">\n'
        for path in files
    )


def _caption_tags(files: list[Path]) -> str:
    return "".join(
        "<figure>\n"
        f'  <img src="./{path.name}" alt="画像の説明" width="" height="" border="0">\n'
        "  <figcaption>画像の説明を記載します。必要に応じて適宜変更してください。</figcaption>\n"
        "</figure>\n"
        for path in files
    )


def _html_document(contents: str, *, charset: str, title: str, style: str = "", description: str = "") -> str:
    return f"""<!doctype html>
<html lang="">
<head>
  <meta charset="{charset}">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>{title}</title>
  <link rel="stylesheet" href="style_index.css">
{style}</head>
<body>

<h1>Hello, world!!</h1>
<h2>Hello, world!!</h2>
<h3>Hello, world!!</h3>
<h4>Hello, world!!</h4>
{description}
<!-- ここから -->
{contents}<!-- ここまで -->

</body>
</html>
"""


def _outputs(files: list[Path]) -> dict[str, str]:
    tags = _image_tags(files)
    captions = _caption_tags(files)
    basic_html = _html_document(
        tags,
        charset="utf-8",
        title="",
        description="<p>\nHello, world!!<br>\nHello, world!!\n</p>\n<p>\nHello, world!!\n</p>\n",
    ).replace(
        '  <link rel="stylesheet" href="style_index.css">\n',
        '  <link rel="stylesheet" href="style_index.css">\n'
        '  <meta name="description" content="">\n'
        '  <meta property="og:title" content="">\n'
        '  <meta property="og:type" content="">\n'
        '  <meta property="og:url" content="">\n'
        '  <meta property="og:image" content="">\n'
        '  <meta property="og:image:alt" content="">\n'
        '  <link rel="icon" href="/favicon.ico" sizes="any">\n'
        '  <link rel="icon" href="/icon.svg" type="image/svg+xml">\n'
        '  <link rel="apple-touch-icon" href="icon.png">\n'
        '  <link rel="manifest" href="site.webmanifest">\n'
        '  <meta name="theme-color" content="#fafafa">\n',
    )
    outer_style = """  <style>
    img {
      border: 2px solid #444;
      padding: 4px;
      background-color: #fff;
      box-shadow: 2px 2px 6px rgba(0,0,0,0.2);
      margin: 1em 0;
      max-width: 100%;
      height: auto;
    }
  </style>
"""
    caption_style = """  <style>
    figure { display: inline-block; text-align: center; margin: 1.5em 0; }
    figure img {
      border: 2px solid #444;
      padding: 4px;
      background-color: #fff;
      box-shadow: 2px 2px 6px rgba(0,0,0,0.2);
      max-width: 100%;
      height: auto;
    }
    figcaption { font-size: 0.9rem; color: #555; margin-top: 0.5em; }
  </style>
"""
    css = """/* style_index.css */
h1, h2, h3, h4 {
  font-family: system-ui, -apple-system, "Segoe UI", Meiryo, "Yu Gothic UI", sans-serif;
  line-height: 1.35;
  color: #222;
  margin: 1.2em 0 0.6em;
}
h1 { font-size: 2.0rem; font-weight: 700; border-left: 0.5rem solid #222; padding-left: 0.6rem; background-image: none; }
h2 { font-size: 1.6rem; font-weight: 700; padding-bottom: 0; border-bottom: none; }
h3 { font-size: 1.3rem; font-weight: 600; padding-bottom: 0; border-bottom: none; }
h4 { font-size: 1.15rem; font-weight: 600; padding: 0.15em 0.4em; background-color: #f4f4f4; border-left: 0.3rem solid #888; }
p { margin: 0 0 1em; line-height: 1.8; }
ul, ol { margin: 0 0 1em 1.5em; }
"""
    return {
        "FileList.txt": tags,
        "FileList_outer_frame.txt": tags,
        "FileList_caption_frame.txt": captions,
        "index.html.txt": basic_html,
        "index_outer_frame.html": _html_document(tags, charset="shift_jis", title="outer frame", style=outer_style, description="<p>このページは、画像に外枠のみを付けたものです。</p>\n"),
        "index_caption_frame.html": _html_document(captions, charset="shift_jis", title="caption + frame", style=caption_style, description="<p>このページは、画像に外枠とキャプションを付けたものです。</p>\n"),
        "style_index.css": css,
    }


def _replace_outputs(output_directory: Path, outputs: dict[str, str]) -> None:
    temporary_files: dict[str, Path] = {}
    try:
        for name, contents in outputs.items():
            descriptor, temporary_name = tempfile.mkstemp(prefix=f".{name}.", dir=output_directory)
            temporary_path = Path(temporary_name)
            temporary_files[name] = temporary_path
            with os.fdopen(descriptor, "w", encoding=OUTPUT_ENCODINGS[name], newline="\n") as stream:
                stream.write(contents)
                stream.flush()
                os.fsync(stream.fileno())
        for name, temporary_path in temporary_files.items():
            os.replace(temporary_path, output_directory / name)
    finally:
        for temporary_path in temporary_files.values():
            temporary_path.unlink(missing_ok=True)


def main(arguments: list[str] | None = None) -> int:
    try:
        files, output_directory = _validate_files(list(sys.argv[1:] if arguments is None else arguments))
        _replace_outputs(output_directory, _outputs(files))
    except (OSError, UnicodeError, ValueError) as error:
        print(f"エラー: {error}", file=sys.stderr)
        return 1
    print(f"出力先: {output_directory}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
