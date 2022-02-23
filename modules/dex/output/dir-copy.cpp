// Copyright (C) 2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/dir-copy.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace dex
{

void recursive_copy(const QString& src, const QString& dest)
{
  QFileInfo src_file_info{ src };

  if (src_file_info.isDir())
  {
    QDir target_dir{ dest };
    target_dir.cdUp();
    target_dir.mkdir(QFileInfo(dest).fileName());

    QDir src_dir{ src };

    QStringList filenames = src_dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& filename : filenames)
    {
      const QString nested_src = src + "/" + filename;
      const QString nested_dest = dest + "/" + filename;
      recursive_copy(nested_src, nested_dest);
    }
  }
  else
  {
    QFile::copy(src, dest);
  }
}

} // namespace dex
