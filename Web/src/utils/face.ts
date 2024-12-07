const module = import.meta.glob("~/assets/face/*.gif", {
  query: "?url",
  import: "default",
  eager: true,
});

const allFace: Record<string, any> = {};
Object.entries(module).forEach(([key, value]) => {
  const name = key.match(/\/(\w+)\.gif/)![1];
  allFace[name] = value;
});

export const getFaceList = () => {
  return Object.keys(allFace).map((item) => {
    let codePoint = parseInt(item, 16); // 将十六进制字符串转换为十进制整数
    return String.fromCodePoint(codePoint);
  });
};

export const getFaceUnicodeList = () => {
  return Object.keys(allFace);
};

export const getFaceUrl = (name: string) => {
  return allFace[name];
};

// unicode转表情字符
export const unicodeToChar = (unicode: string) => {
  const codePoint = parseInt(unicode, 16); // 将十六进制字符串转换为十进制整数
  return String.fromCodePoint(codePoint);
};
