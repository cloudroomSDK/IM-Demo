//
//  FileForUrlCache.swift
//  Alamofire
//
//  Created by x on 2023/5/11.
//

import Foundation

public struct CacheableFileKey: Hashable, PersistentlyCacheable {

    public let url: URL

    var persistentIdentifier: String {
        url.absoluteString.addingPercentEncoding(withAllowedCharacters: .alphanumerics) ?? url.absoluteString
    }
}

public final class FileForUrlCache<Cache: AsyncKeyValueCaching>: AsyncKeyValueCaching where Cache.CachingKey: Hashable, Cache.Entity == Data {

    private let cache: Cache

    public init(cache: Cache) {
        self.cache = cache
    }

    public func isEntityCached(for key: CachingKey) -> Bool {
        cache.isEntityCached(for: key)
    }

    public func getEntity(for key: CachingKey) throws -> String {
        let data = try cache.getEntity(for: key)
        guard let filePath = String(data: data, encoding: .utf8) else {
            throw CacheError.invalidData
        }
        return filePath
    }

    public func getEntity(for key: Cache.CachingKey, completion: @escaping (Result<String, Error>) -> Void) {
        cache.getEntity(for: key, completion: { result in
            DispatchQueue.global(qos: .utility).async {
                switch result {
                case let .success(data):
                    guard let filePath = String(data: data, encoding: .utf8) else {
                        DispatchQueue.main.async {
                            completion(.failure(CacheError.invalidData))
                        }
                        return
                    }
                    DispatchQueue.main.async {
                        completion(.success(filePath))
                    }
                case let .failure(error):
                    DispatchQueue.main.async {
                        completion(.failure(error))
                    }
                }
            }
        })
    }

    public func store(entity: String, for key: Cache.CachingKey) throws {
        guard let data = entity.data(using: .utf8) else {
            return
        }
        try cache.store(entity: data, for: key)
    }

}
