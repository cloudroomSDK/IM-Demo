
import Foundation

@available(iOS 13, *)
var metadataCache = IterativeCache(mainCache: MetaDataCache(cache: MemoryDataCache<URL>()),
                                   backupCache: MetaDataCache(cache: PersistentDataCache<URL>(cacheFileExtension: "metadataCache")))

let imageCache = IterativeCache(mainCache: ImageForUrlCache(cache: MemoryDataCache<CacheableImageKey>()),
                                backupCache: ImageForUrlCache(cache: PersistentDataCache<CacheableImageKey>()))

let fileCache = IterativeCache(mainCache: FileForUrlCache(cache: MemoryDataCache<CacheableFileKey>()),
                                backupCache: FileForUrlCache(cache: PersistentDataCache<CacheableFileKey>()))
