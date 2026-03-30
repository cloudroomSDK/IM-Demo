import { dayjs } from "element-plus";

// 扩展插件
import calendar from "dayjs/plugin/calendar";
import duration from "dayjs/plugin/duration"; // ES 2015
import "dayjs/locale/zh-cn";
dayjs.extend(calendar);
dayjs.extend(duration);
dayjs.locale("zh-cn");

export type TimeType = string | number | dayjs.Dayjs | Date | null | undefined;
export function formatChatString(
  time: TimeType,
  format: string = "YYYY-MM-DD HH:mm",
) {
  return dayjs(time).calendar(null, {
    sameDay: "HH:mm",
    lastDay: "昨天 HH:mm",
    // nextDay: format,
    // nextWeek: format,
    lastWeek: format,
    sameElse: format,
  });
}

export function formatListString(time: TimeType, format: string = "M月D日") {
  return dayjs(time).calendar(null, {
    sameDay: "HH:mm",
    lastDay: "昨天",
    // nextDay: format,
    // nextWeek: format,
    lastWeek: "dddd",
    sameElse: format,
  });
}

export function formatString(
  time: TimeType,
  format: string = "YYYY-MM-DD HH:mm",
) {
  return dayjs(time).format(format);
}

//格式化持续时间
export function formatDuration(duration: number, format: string = "mm:ss") {
  return dayjs.duration(duration, "s").format(format);
}
