#include <QtCore>
#include <QtGui>
#include <QtSql>
#include "library/trackcollection.h"
#include "library/missingtablemodel.h"


MissingTableModel::MissingTableModel(QObject* parent,
                                       TrackCollection* pTrackCollection)
        : TrackModel(),
          QSqlTableModel(parent, pTrackCollection->getDatabase()),
          m_pTrackCollection(pTrackCollection),
          m_trackDao(m_pTrackCollection->getTrackDAO()),
          m_currentSearch("") {

    QSqlQuery query;
    //query.prepare("DROP VIEW " + playlistTableName);
    //query.exec();
    QString tableName("missing_songs");

    query.prepare("CREATE TEMPORARY VIEW " + tableName + " AS "
                  "SELECT " +
                  "library." + LIBRARYTABLE_ID + "," +
                  "library." + LIBRARYTABLE_ARTIST + "," +
                  "library." + LIBRARYTABLE_TITLE + "," +
                  "track_locations.location" + "," +
                  "library." + LIBRARYTABLE_ALBUM + "," +
                  "library." + LIBRARYTABLE_YEAR + "," +
                  "library." + LIBRARYTABLE_DURATION + "," +
                  "library." + LIBRARYTABLE_GENRE + "," +
                  "library." + LIBRARYTABLE_TRACKNUMBER + "," +
                  "library." + LIBRARYTABLE_BPM + "," +
                  "library." + LIBRARYTABLE_LOCATION + "," +
                  "library." + LIBRARYTABLE_COMMENT + " "
                  "FROM library "
                  "INNER JOIN track_locations "
                  "ON library.location=track_locations.id "
                  "WHERE track_locations.fs_deleted=1 ");
    //query.bindValue(":playlist_name", playlistTableName);
    //query.bindValue(":playlist_id", m_iPlaylistId);
    if (!query.exec()) {
        qDebug() << query.executedQuery() << query.lastError();
    }

    qDebug() << query.executedQuery();

    //Print out any SQL error, if there was one.

    if (query.lastError().isValid()) {
     	qDebug() << __FILE__ << __LINE__ << query.lastError();
    }

    setTable(tableName);

    qDebug() << "Created MissingTracksModel!";

	//Set the column heading labels, rename them for translations and have proper capitalization
    setHeaderData(fieldIndex("location"),
                  Qt::Horizontal, tr("Location"));
    setHeaderData(fieldIndex(LIBRARYTABLE_ARTIST),
                  Qt::Horizontal, tr("Artist"));
    setHeaderData(fieldIndex(LIBRARYTABLE_TITLE),
                  Qt::Horizontal, tr("Title"));
    setHeaderData(fieldIndex(LIBRARYTABLE_ALBUM),
                  Qt::Horizontal, tr("Album"));
    setHeaderData(fieldIndex(LIBRARYTABLE_GENRE),
                  Qt::Horizontal, tr("Genre"));
    setHeaderData(fieldIndex(LIBRARYTABLE_YEAR),
                  Qt::Horizontal, tr("Year"));
    setHeaderData(fieldIndex(LIBRARYTABLE_LOCATION),
                  Qt::Horizontal, tr("Location"));
    setHeaderData(fieldIndex(LIBRARYTABLE_COMMENT),
                  Qt::Horizontal, tr("Comment"));
    setHeaderData(fieldIndex(LIBRARYTABLE_DURATION),
                  Qt::Horizontal, tr("Duration"));
    setHeaderData(fieldIndex(LIBRARYTABLE_TRACKNUMBER),
                  Qt::Horizontal, tr("Track #"));
    setHeaderData(fieldIndex(LIBRARYTABLE_BITRATE),
                  Qt::Horizontal, tr("Bitrate"));
    setHeaderData(fieldIndex(LIBRARYTABLE_BPM),
                  Qt::Horizontal, tr("BPM"));

    select(); //Populate the data model.

}

MissingTableModel::~MissingTableModel() {
}

void MissingTableModel::addTrack(const QModelIndex& index, QString location)
{
    //Note: The model index is ignored when adding to the library track collection.
    //      The position in the library is determined by whatever it's being sorted by,
    //      and there's no arbitrary "unsorted" view.
}

TrackInfoObject* MissingTableModel::getTrack(const QModelIndex& index) const
{
    //FIXME: use position instead of location for playlist tracks?

    //const int locationColumnIndex = this->fieldIndex(LIBRARYTABLE_LOCATION);
    //QString location = index.sibling(index.row(), locationColumnIndex).data().toString();
    int trackId = index.sibling(index.row(), fieldIndex(LIBRARYTABLE_ID)).data().toInt();
    return m_trackDao.getTrack(trackId);
}

QString MissingTableModel::getTrackLocation(const QModelIndex& index) const
{
    //FIXME: use position instead of location for playlist tracks?
    int trackId = index.sibling(index.row(), fieldIndex(LIBRARYTABLE_ID)).data().toInt();
    QString location = m_trackDao.getTrackLocation(trackId);
    return location;
}

void MissingTableModel::removeTrack(const QModelIndex& index)
{
}

void MissingTableModel::moveTrack(const QModelIndex& sourceIndex, const QModelIndex& destIndex)
{
}

void MissingTableModel::search(const QString& searchText)
{
    //FIXME: Need to keep filtering by playlist_id too
    //SQL is "playlist_id = " + QString(m_iPlaylistId)
    m_currentSearch = searchText;
    if (searchText == "")
        this->setFilter("");
    else
        this->setFilter("artist LIKE \'%" + searchText + "%\' OR "
                        "title  LIKE \'%" + searchText + "%\'");
}

const QString MissingTableModel::currentSearch() {
    return m_currentSearch;
}

bool MissingTableModel::isColumnInternal(int column) {
    if (column == fieldIndex(LIBRARYTABLE_ID))
        return true;
    else
        return false;
}

QMimeData* MissingTableModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *mimeData = new QMimeData();
    QList<QUrl> urls;

    //Ok, so the list of indexes we're given contains separates indexes for
    //each column, so even if only one row is selected, we'll have like 7 indexes.
    //We need to only count each row once:
    QList<int> rows;

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            if (!rows.contains(index.row())) {
                rows.push_back(index.row());
                QUrl url(getTrackLocation(index));
                if (!url.isValid())
                    qDebug() << "ERROR invalid url\n";
                else
                    urls.append(url);
            }
        }
    }
    mimeData->setUrls(urls);
    return mimeData;
}

Qt::ItemFlags MissingTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if (!index.isValid())
      return Qt::ItemIsEnabled;

    //defaultFlags |= Qt::ItemIsDragEnabled;
    defaultFlags = 0;

    return defaultFlags;
}

QItemDelegate* MissingTableModel::delegateForColumn(int i) {
    return NULL;
}

QVariant MissingTableModel::data(const QModelIndex& item, int role) const {
    if (!item.isValid())
        return QVariant();

    QVariant value = QSqlTableModel::data(item, role);

    if (role == Qt::DisplayRole &&
        item.column() == fieldIndex(LIBRARYTABLE_DURATION)) {
        if (qVariantCanConvert<int>(value)) {
            // TODO(XXX) Pull this out into a MixxxUtil or something.

            //Let's reformat this song length into a human readable MM:SS format.
            int totalSeconds = qVariantValue<int>(value);
            int seconds = totalSeconds % 60;
            int mins = totalSeconds / 60;
            //int hours = mins / 60; //Not going to worry about this for now. :)

            //Construct a nicely formatted duration string now.
            value = QString("%1:%2").arg(mins).arg(seconds, 2, 10, QChar('0'));
        }
    }
    return value;
}

TrackModel::CapabilitiesFlags MissingTableModel::getCapabilities() const
{
    return 0;
}
